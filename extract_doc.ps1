$word = New-Object -ComObject Word.Application
$word.Visible = $false
$doc = $word.Documents.Open((Get-Item 'Farewell_Trip.doc').FullName)
$text = $doc.Content.Text
$doc.Close($false)
$word.Quit()
[System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
Write-Output $text
