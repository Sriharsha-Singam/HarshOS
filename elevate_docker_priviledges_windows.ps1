$User_Account = Read-Host -Prompt 'Input your account name: '
Install-Module -Name dockeraccesshelper -Confirm
Add-AccountToDockerAccess $User_Account
write-host "Press any key to continue..."
[void][System.Console]::ReadKey($true)