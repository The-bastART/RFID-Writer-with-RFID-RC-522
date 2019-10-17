$port=new-Object System.IO.Ports.SerialPort Com3, 9600, None, 8, one
$port.open()
while(1){
    $data=Read-Host -Prompt 'Bitte den Heimausweis scannen'
    $port.WriteLine($data)
	write-host "Karte auflegen und zwei Sekunden warten."
	write-host "Beliebige Taste drücken um fortzufahren."
	[void][System.Console]::ReadKey($true)
	$data1=Read-Host -Prompt 'Bitte den programmierten Heimausweis auflegen'
	if ($data1 -eq $data) {
		write-host "IDs sind identisch."
	}  else {
		write-host "IDs sind nicht identisch."
	}
	write-host "Beliebige Taste drücken um fortzufahren."
	[void][System.Console]::ReadKey($true)
	cls
}