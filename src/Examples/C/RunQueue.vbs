' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
' 
' This code is in the public domain.
'
' This program is distributed in the hope that it will be useful, but
' WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
' implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Option Explicit

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Declaration of variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Dim rc
Dim objShell
Dim fso

Dim exeName, tmpDir, cmdFile
Dim objArgs
Dim isoFile
Dim quasarAddr
Dim numIterations
Dim milliSecs
Dim cycle

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Initialization of (some of) these variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

Set objShell = CreateObject("WScript.Shell")

Set objArgs = WScript.Arguments

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the program
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

If objArgs.Count <> 5 Then
   Wscript.Echo "usage: cscript RunQueue.vbs iso_data_file quasar_address number_of_iterations millisecs iterations_per_cycle"
   Wscript.Quit(1)
End If
isoFile       = objArgs(0)
quasarAddr    = objArgs(1)
numIterations = objArgs(2)
milliSecs     = objArgs(3)
cycle         = objArgs(4)

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start quasar (the same server for all the test in this sub-dir).
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\queue"

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if

fso.CreateFolder(tmpDir)

fso.Copyfile "..\quasar_config.xsd", tmpDir + "\quasar_config.xsd"

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.xml", True)
cmdFile.WriteLine("<?xml version=""1.0""?>")
cmdFile.WriteLine("<quasar_config xmlns=""http://photonsoftware.org/quasarConfig""")
cmdFile.WriteLine("xmlns:xsi=""http://www.w3.org/2001/XMLSchema-instance""")
cmdFile.WriteLine("xsi:schemaLocation=""http://photonsoftware.org/quasarConfig " + tmpDir + "\quasar_config.xsd""> ")
cmdFile.WriteLine("  <mem_location>" + tmpDir + "</mem_location>")
cmdFile.WriteLine("  <mem_size size=""10240"" units=""kb"" />")
cmdFile.WriteLine("  <quasar_address>" + quasarAddr + "</quasar_address>")
cmdFile.WriteLine("  <file_access access=""group"" />")
cmdFile.WriteLine("</quasar_config>")
cmdFile.Close

' Run the Programs

exeName = "quasar.exe -c " + tmpDir + "\cfg.xml"

rc = objShell.Run("%comspec% /c title quasar | " & exeName, 1, false)
Wscript.Sleep 1000

exeName = "QueueIn.exe " + isoFile + " " + quasarAddr + " " + numIterations + _
   " " + milliSecs + " " + cycle
objShell.Run "%comspec% /k title QueueIn | " & exeName, 1, false
Wscript.Sleep 1000

exeName = "QueueWork.exe " + quasarAddr
objShell.Run "%comspec% /k title QueueWork | " & exeName, 1, false

exeName = "QueueOut.exe " + quasarAddr
objShell.Run "%comspec% /k title QueueOut | " & exeName, 1, false

wscript.echo "You MUST kill the quasar terminal when the test is over to repeat it"

wscript.quit(0)

