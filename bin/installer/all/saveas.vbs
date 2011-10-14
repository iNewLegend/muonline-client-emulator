Dim WshShell
Set WshShell = WScript.CreateObject("WScript.Shell") 
Set objArgs = WScript.Arguments

WshShell.SendKeys "^s" 
WshShell.SendKeys objArgs(0)
WshShell.SendKeys "{enter}"

