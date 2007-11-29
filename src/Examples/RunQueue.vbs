' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
Dim objWshScriptExec
Dim fso

Dim exe_name, prog_path, program, wd_path, tmpDir, cmdFile, exeName
Dim objArgs, i
dim strOutput
Dim iso_file
Dim wd_addr
Dim num_iterations
Dim ms
Dim cycle

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Initialization of (some of) these variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

'consoleMode = True
'If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
'   consoleMode = False
'End If

Set objShell = CreateObject("WScript.Shell")
'Set objShellwd = CreateObject("WScript.Shell")
'verbose = False

prog_path = "Release"
wd_path = "..\Release"
Set objArgs = WScript.Arguments

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the program
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

If objArgs.Count <> 5 Then
   Wscript.Echo "usage: cscript RunQueue.vbs iso_data_file watchdog_address number_of_iterations millisecs iterations_per_cycle"
   Wscript.Quit(1)
End If
iso_file       = objArgs(0)
wd_addr        = objArgs(1)
num_iterations = objArgs(2)
ms             = objArgs(3)
cycle          = objArgs(4)

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the watchdog (the same wd for all the test in this sub-dir).
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\queue"

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if

fso.CreateFolder(tmpDir)

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.txt", True)
cmdFile.WriteLine("# VDSF Config file             ")
cmdFile.WriteLine("#                              ")
cmdFile.WriteLine("VDSLocation           " + tmpDir)
cmdFile.WriteLine("#MemorySize is in kbytes       ")
cmdFile.WriteLine("MemorySize            10000    ")
cmdFile.WriteLine("WatchdogAddress       10701    ")
cmdFile.WriteLine("LogTransaction        0        ")
cmdFile.WriteLine("FilePermissions       0660     ")
cmdFile.WriteLine("DirectoryPermissions  0770     ")

' Run the Programs

exeName = wd_path + "\vdswd.exe -c " + tmpDir + "\cfg.txt"

objShell.Run "%comspec% /c title vdswd | " & exeName, 1, false
Wscript.Sleep 1000

exeName = "QueueIn.exe " + iso_file + " " + wd_addr + " " + num_iterations + _
   " " + ms + " " + cycle
objShell.Run "%comspec% /k title QueueIn | " & exeName, 1, false
Wscript.Sleep 1000

exeName = "QueueWork.exe " + wd_addr
objShell.Run "%comspec% /k title QueueWork | " & exeName, 1, false

exeName = "QueueOut.exe " + wd_addr
objShell.Run "%comspec% /k title QueueOut | " & exeName, 1, false

wscript.echo "You MUST kill the vdswd terminal when the test is over to repeat it"

wscript.quit(0)

