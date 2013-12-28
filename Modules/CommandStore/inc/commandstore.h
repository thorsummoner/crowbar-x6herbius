/** \file commandstore.h
 * \brief Defines command callbacks usable by console commands.
 */

/**
 * \defgroup CommandStore CommandStore library
 * @{
 */

#ifndef COMMANDSTORE_H
#define COMMANDSTORE_H

#include "commandstore_global.h"
#include "commandsenderinfo.h"
#include "wr_concommand.h"
#include "wr_commandmanager.h"
#include "wr_listedcommandmanager.h"
#include "commandstore_manager.h"

namespace NCommandStore
{
    /**
     * @brief Echoes arguments back to the caller's console and to the output.
     * 
     * Each individual argument is separated by a space.
     * Quoted strings are treated as one argument.
     * To print quotes, escape them with a backslash.
     * @param info Information about the calling command manager.
     * @param args Arguments to this callback.
     * @param output Output variant.
     * @return Return code representing the success or failure of this callback's execution.
     */
    COMMANDSTORESHARED_EXPORT int ccb_Echo(const NIConsole::CommandSenderInfo &info, const QStringList &args, QVariant &output);
    static NIConsole::ConCommand echo("echo", ccb_Echo, g_pCommandManager, &g_pConsoleCommandList,
                           "Echoes arguments to console and output variant.");
    
    /**
     * @brief Outputs messages to test output types and colours for the current style.
     * @param info Information about the calling command manager.
     * @param args Arguments to this callback.
     * @param output Output variant.
     * @return Return code representing the success or failure of this callback's execution.
     */
    COMMANDSTORESHARED_EXPORT int ccb_Con_TestColours(const NIConsole::CommandSenderInfo &info, const QStringList &args, QVariant &output);
    static NIConsole::ConCommand con_testcolours("con_testcolours", ccb_Con_TestColours, g_pCommandManager, &g_pConsoleCommandList,
                                      "Outputs test phrases for each of the 8 different supported console message colours");
}

#endif // COMMANDSTORE_H

/**@}*/
