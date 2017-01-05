Get-ChildItem -Path "C:\Users\slang\Dropbox\Arduino\_Code\iSpindel\pics\PCB" | Rename-Item -NewName {$_.Name.Replace("--","-")}

Get-ChildItem -Path "C:\Users\slang\Dropbox\Arduino\_Code\iSpindel\pics\PCB" | % {"![{0}](../pics/PCB/{0})" -f $_.Name}