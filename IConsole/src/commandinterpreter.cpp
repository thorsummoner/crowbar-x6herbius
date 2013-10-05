#include "wr_commandinterpreter.h"

CommandInterpreter::CommandInterpreter(QObject *parent) :
    QObject(parent)
{
}

CommandInterpreter::CommandInterpreter(CommandManager *manager, QObject *parent) :
    QObject(parent), m_pCommandManager(manager)
{
}

void CommandInterpreter::setManager(CommandManager *manager)
{
    m_pCommandManager = manager;
}

CommandManager* CommandInterpreter::getManager() const
{
    return m_pCommandManager;
}

void CommandInterpreter::getSuggestions(const QString &prefix, QList<CommandIdentPair> &list)
{
    // If manager is NULL, return.
    if ( !m_pCommandManager ) return;
    
    // Call findPrefix on the manager.
    QList<BaseConsoleCommand*> cmdList;
    m_pCommandManager->findPrefix(prefix, cmdList);
    
    // For each of the returned commands, create an entry in our output list.
    foreach (BaseConsoleCommand* cmd, cmdList)
    {
        list.append(QPair<NGlobalCmd::CmdIdent, QString>(cmd->identify(), cmd->getName()));
    }
}

void CommandInterpreter::parse(const QString &cmdString)
{
    // Steps:
    // - Create a list to hold <QString, QStringList> pairs. The QString is the command name, the list is its arguments.
    // - If pipes are present, the list will be longer than a single element. If a command is to receive piped output from
    //      a previous command, a $ symbol should signify the point in the arguments list to insert the received output.
    //      The $ symbol should be on its own as a single argument.
    //      If no $ argument is found, the arguments are appended to the end of the existing argument list.
    //      If multiple $ argument are found, only the first is used.
    //      Output is inserted into the argument list as a zero or more strings. These strings are inserted exactly as they are presented
    //      from the previous command - if the output is a string list, each list entry will become a new argument; if the output is one
    //      long string, the string will become one argument without being delimited at any characters. Arguments output as numbers
    //      (int/float/etc.) are cast to a string before being passed.
    // - Once the list has been compiled, each command should be executed sequentially.
    
    // If we have no manager, don't bother running this.
    if ( !m_pCommandManager ) return;
    
    // Create a command entry list.
    CommandEntryList masterList;
    
    // Parse our command string.
    parseCommandString(cmdString, masterList);
    
//    qDebug() << "Number of pipe lists: " << masterList.count();
    
    // Iterate through the pipe lists:
    foreach(CommandEntryPipeList pipeList, masterList)
    {
//        qDebug() << "Number of command pairs in this pipe list: " << pipeList.count();
        
        // String list of arguments we need to pipe to the next command in the pipe list.
        QStringList pipeArgs;
        
        // For each of the commands:
        foreach(CommandEntryPair pair, pipeList)
        {
//            qDebug() << "Command in this pair: " << pair.first;
//            qDebug() << "Args for this pair: " << pair.second.count();
//            foreach(QString s, pair.second)
//            {
//                qDebug() << s;
//            }
            
            int returnVal = NGlobalCmd::CCR_OK;
            QVariant output;
            
            // Insert stored output if we need to.
            if ( pipeArgs.count() > 0 )
            {
                // Find the index of the first single argument comprised of "$".
                // We allow for whitespace here.
                int index = pair.second.indexOf(QRegularExpression("^\\s*\\$\\s*$"));
                
                // If the index existed:
                if ( index >= 0 )
                {
                    // Remove the index of "$" first.
                    pair.second.removeAt(index);
                    
                    foreach(QString str, pipeArgs)
                    {
                        // Insert at the index.
                        pair.second.insert(index, str);
                        
                        // Increment the index for the next insertion.
                        index++;
                    }
                }
                // Otherwise we need to append to the list instead.
                else
                {
                    foreach(QString str, pipeArgs)
                    {
                        // Append.
                        pair.second.append(str);
                    }
                }
            }
            
            // If the command was found successfully:
            if ( m_pCommandManager->exec(pair.first, pair.second, returnVal, output) != NGlobalCmd::CINull )
            {
                // Convert the output to a string list.
                pipeArgs = output.toStringList();
                
                // Not sure at this stage whether toStringList will work with values that aren't strings,
                // so I'm going to workaround if the value didn't copy over.
                // Might be a bit hackish, keep an eye on this.
                if ( pipeArgs.count() < 1 )
                {
                    // If toString doesn't work, toStringList wouldn't have worked anyway.
                    pipeArgs = QStringList(output.toString());
                }
            }
            // Otherwise, blank the output.
            else
            {
                pipeArgs.clear();
            }
        }
    }
}

// Given a command string, filled the passed list with the commands and arguments contained in the string.
void CommandInterpreter::parseCommandString(const QString &cmdString, CommandEntryList &masterList)
{
    // List to hold command strings.
    // The structure of this list is:
    /*
     * List <- Main list. Sub-lists were delimited by semicolons.
     * {
     *      List    <- Sub-list. Sequential commands pipe output to input. Commands in this list were delimited by pipes.
     *      {
     *          Pair(string, StringList)    <- Pair of command string and list of arguments. Executed first.
     *          Pair(string, StringList)    <- Executed second. Receives input of first.
     *          ...
     *      }
     *
     *      List
     *      {
     *          Pair(String, StringList)    <- Executed next. Does not receive any input.
     *          Pair(string, StringList)    <- Executed next again, receiving above command's input.
     *          ...
     *      }
     *
     *      ...
     * }
     */
    
    // TODO: Put this in the documentation!
    // Unescaped reged to search a command string for pipes.
    // Will match unescaped pipe characters, regardless of how much whitespace they are (or are not) surrounded by.
    // \s*(?<!\\)\|\s*
    // Searches for 0 or more whitespace characters, followed by a pipe which is not preceded by a backslash (using negative
    // lookbehind), followed by 0 or more whitespace characters.
    QRegularExpression delimPipes = QRegularExpression("\\s*(?<!\\\\)\\|\\s*");
    
    // Unescaped regex to search a command string for semicolons.
    // \s*(?<!\\)\;\s*
    // Similar construction to above.
    QRegularExpression delimSemicolons = QRegularExpression("\\s*(?<!\\\\)\\;\\s*");
    
    // Unescaped regex to search a command string for arguments.
    // Arguments are either space-delimited or enclosed in unescaped quotes.
    // "[^"\\]*(?:\\.[^"\\]*)*"|[\S]+
    
    // For example, when parsing the following string:
    // this is a \" test\" "command \"string\""
    
    // The matches are:
    // this
    // is
    // a
    // \"
    // test\"
    // "command \"string\""
    QRegularExpression matchArgs = QRegularExpression("\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"|[\\S]+");
    
//    qDebug() << "Original string to parse: " << cmdString;
    
    // Split the original string by semicolons.
    QStringList splitFirstPass = cmdString.split(delimSemicolons);
    
//    qDebug() << "Split strings, first pass: ";
//    foreach(QString s, splitFirstPass)
//    {
//        qDebug() << s;
//    }
    
    // Operate on each entry.
    foreach(QString str1, splitFirstPass)
    {
        // Create a list of QPairs.
        CommandEntryPipeList pipeList;
        
        // Split each new string by pipes.
        QStringList splitSecondPass = str1.split(delimPipes);
        
//        qDebug() << "Split strings, second pass: ";
//        foreach(QString s, splitSecondPass)
//        {
//            qDebug() << s;
//        }
        
        // Operate on each new entry.
        foreach (QString str2, splitSecondPass)
        {
            // Create a new pair for this command.
            CommandEntryPair pair;
            
            // Retrieve the list of argument matches.
            QRegularExpressionMatchIterator m = matchArgs.globalMatch(str2);
            
            while ( m.hasNext() )
            {
                pair.second.append(m.next().captured(0));
            }
            
//            qDebug() << "Number of arguments matched from" << str2 << ":" << pair.second.count();
//            foreach(QString s, pair.second)
//            {
//                qDebug() << s;
//            }
            
            // If the number of arguments found is < 1, insert a blank command into the list.
            if ( pair.second.count() < 1 )
            {
                pair.first = QString("");
            }
            // Otherwise, set the command name to be the first in the argument list.
            else
            {
                pair.first = pair.second.takeAt(0);
            }
            
            // Add the pair to the pipe list.
            pipeList.append(pair);
        }
        
        // Add the list of pairs to the main list.
        masterList.append(pipeList);
    }
}