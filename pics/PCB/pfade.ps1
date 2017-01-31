Get-ChildItem -Path "C:\Users\slang\Dropbox\Arduino\_Code\iSpindel\pics\PCB" | Rename-Item -NewName {$_.Name.Replace("--","-").Replace(" ","").Replace("ü","ue").Replace("ä","ae").Replace("ö","oe")}
Get-ChildItem -Path "C:\Users\slang\Dropbox\Arduino\_Code\iSpindel\pics\PCB" | Rename-Item -NewName  {$_.Name -replace "^([0-9]{1})-",'0$1-'}

Get-ChildItem -Path "C:\Users\slang\Dropbox\Arduino\_Code\iSpindel\pics\PCB" | % {"![{0}](../pics/PCB/{0})" -f $_.Name} | clip