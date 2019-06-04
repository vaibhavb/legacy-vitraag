Get-Service "*Amalga*" | Stop-Service -Force # Stop all services with 'Amalga' in their names.
Get-Service "ase" | Stop-Service -Force
Stop-Service "Azyxxi.Update" -Force
Get-Service -DisplayName "*SQL Server (*" | Stop-Service -Force # Stop all SQL Server instances.
Stop-Service "MSMQ" -Force
Stop-Service "W3SVC" -Force


Start-Service "MSMQ"
Get-Service -DisplayName "*SQL Server (*" | Start-Service # Start all SQL Server instances. 
Start-Service "W3SVC"
Get-Service "*Amalga*" | Where-Object {$_.Name -like '*Receiver*'} | Start-Service # Start all Receivers.
Get-Service "*Amalga*" | Where-Object {$_.Name -like '*Filer*'} | Start-Service # Start all Filers.
Get-Service "*Amalga*" | Where-Object {$_.Name -like '*Deleter*'} | Start-Service # Start all Deleters.
Start-Service "Ase"
Start-Service "Azyxxi.Update"
Get-Service "*Dicom*" | Start-Service # Start any services with 'Dicom' in their names. It's OK if they're already been started.
Get-Service "*Amalga*" | Start-Service # Start any services with 'Amalga' in their names. It's OK if they're already been started.

Echo Completed restarting services.
