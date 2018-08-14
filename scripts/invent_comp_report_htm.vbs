'Инвентаризация компьютеров средствами WMI и VBScript (11.06.2009)
'Сборка общего HTML-отчета по всем компьютерам из отдельных CSV-файлов
'Подробности - см. http://zheleznov.info/invent_comp.htm
'Автор: Павел Железнов

'== НАСТРОЙКИ

Const TITLE = "Инвентаризация компьютеров" 'заголовок отчета и диалоговых окон
Const DATA_DIR = "comp\" 'каталог для сохранения отчетов + "\" в конце
'Const DATA_DIR = "\\SRV\Invent\comp\" 'сетевой ресурс для сохранения отчетов + "\" в конце
Const DATA_EXT = ".csv" 'расширение файлов с данными
Const HEAD_LINE = True 'пропустить первую строку в файле CSV - заголовок
Const REPORT_FILE = "comp_report_%DATE%.htm" 'имя файла для сохранения отчета

'количество, порядок и названия столбцов отчета
'значения должны соответствовать первым двум полям CSV файла!
Dim col(22) '<-- не забыть проверить верхний индекс!
col(0) = "Компьютер;Сетевое имя"
col(1) = "Компьютер;UUID"
col(2) = "Компьютер;Текущий пользователь"
col(3) = "Операционная система;Наименование"
col(4) = "Операционная система;Обновление"
col(5) = "Материнская плата;Производитель"
col(6) = "Материнская плата;Наименование"
col(7) = "Процессор;Наименование"
col(8) = "Процессор;Частота (МГц)"
col(9) = "Компьютер;Объем памяти (Мб)"
col(10) = "Модуль памяти;Тип"
col(11) = "Модуль памяти;Размер (Мб)"
col(12) = "Модуль памяти;Частота"
col(13) = "Диск;Наименование"
col(14) = "Диск;Размер (Гб)"
col(15) = "Диск;Интерфейс"
col(16) = "CD-привод;Наименование"
col(17) = "Видеоконтроллер;Наименование"
col(18) = "Видеоконтроллер;Объем памяти (Мб)"
col(19) = "Сетевой адаптер;Наименование"
col(20) = "Сетевой адаптер;MAC-адрес"
col(21) = "Звуковое устройство;Наименование"
col(22) = "Принтер;Наименование"

'шапка и подвал отчета в формате XHTML
'оформление настраивается через CSS внутри тега <style>
Dim header, footer

header = "<html><head>" _
	& "<title>" & TITLE & "</title>" & vbCrLf _
	& "<meta http-equiv=""Content-Type"" content=""text/html; charset=windows-1251"" />" & vbCrLf _
	& "<style><!--" & vbCrLf _
	& "body,table {font: 10pt Arial, sans-serif}" & vbCrLf _
	& "table {border-collapse: collapse}" & vbCrLf _
	& "tr,td,th {border: 1px solid gray; padding: 8px}" & vbCrLf _
	& "td {vertical-align: top}" & vbCrLf _
	& "--></style>" & vbCrLf _
	& "</head><body>" & vbCrLf _
	& "<h3>" & TITLE & ", " & Date & "</h3>" & vbCrLf _
	& "<table>" & vbCrLf

footer = "</table>" & vbCrLf _
	& "</body></html>"

'не завершать скрипт аварийно
On Error Resume Next

'== ВЫПОЛНЕНИЕ

'файл отчета
Dim fso, report
Set fso = CreateObject("Scripting.FileSystemObject")
report = Replace(REPORT_FILE, "%DATE%", Date)
Set rep = fso.CreateTextFile(report, True)
rep.Write header

'шапка таблицы
rep.WriteLine "<tr><th>" & Replace(Join(col, "</th><th>"), ";", ":<br />") & "</th></tr>"

'прочитать все CSV-файлы
Dim dir, fc, f, row
Set dir = fso.GetFolder(DATA_DIR)
Set fc = dir.Files
For Each f in fc
	If Right(f.Name, 4) = DATA_EXT Then row = ReadCSV(dir.Path & "\" & f.Name)
	If Len(row) > 0 Then rep.WriteLine row
Next

'закрыть файл отчета
rep.Write footer
rep.Close
MsgBox "Отчет сохранен в файл:" & vbCrLf & report, vbInformation, TITLE

'== ПОДПРОГРАММЫ

'определить индекс элемента массива по его значению
'если значение не найдено, вернуть -1
Function IndexCol(s)
	IndexCol = -1
	Dim i
	For i = 0 To UBound(col)
		If col(i) = s Then
			IndexCol = i
			Exit For
		End If
	Next
End Function

'типы оперативной памяти
Function TypeMemory(s)
Select Case s
   Case "0"
    TypeMemory = " DDR3-?"
   Case "1"
    TypeMemory = " Other"
   Case "2"
    TypeMemory = " DRAM"
   Case "3"
    TypeMemory = " Synchronous DRAM"
   Case "4"
    TypeMemory = " Cache DRAM"
   Case "5"
    TypeMemory = " EDO"
   Case "6"
    TypeMemory = " EDRAM"
   Case "7"
    TypeMemory = " VDRAM"
   Case "8"
    TypeMemory = " SRAM"
   Case "9"
    TypeMemory = " RAM"
   Case "10"
    TypeMemory = " ROM"
   Case "11"
    TypeMemory = " Flash"
   Case "12"
    TypeMemory = " EEPROM"
   Case "13"
    TypeMemory = " FEPROM"
   Case "14"
    TypeMemory = " EPROM"
   Case "15"
    TypeMemory = " CDRAM"
   Case "16"
    TypeMemory = " 3DRAM"
   Case "17"
    TypeMemory = " SDRAM"
   Case "18"
    TypeMemory = " SGRAM"
   Case "19"
    TypeMemory = " RDRAM"
   Case "20"
    TypeMemory = " DDR"
   Case "21"
    TypeMemory = " DDR-2"
 End Select
End Function

'извлечь данные из файла CSV
'сформировать и вернуть строку таблицы в формате XHTML
'в случае ошибки вернуть пустую строку
Function ReadCSV(fname)
	Dim tf, s, a, k, i
	Dim v()
	ReDim v(UBound(col))
	'значение по умолчанию - "-"
	For i = 0 To UBound(v)
		v(i) = "-"
	Next
	Set tf = fso.OpenTextFile(fname, 1, False, True)
	If HEAD_LINE Then tf.ReadLine 'пропустить первую строку
	Do Until tf.AtEndOfStream
		s = tf.ReadLine
		a = Split(s, ";")
		k = a(0) & ";" & a(1)
		i = IndexCol(k)
		If i > -1 Then
			If a(2) > 1 Then 'несколько экземпляров разделяются ";"
				If i = 10 Then
					v(i) = v(i) & ";" & TypeMemory(a(3))
				Else
					v(i) = v(i) & ";" & a(3)
				End If
			Else
				If i = 10 Then
					v(i) = TypeMemory(a(3))
				Else
					v(i) = a(3)
				End If
				
			End If
		End If
	Loop
	tf.Close
	'несколько экземпляров оформить списком в формате XHTML
	For i = 0 To UBound(v)
		'If InStr(v(i), ";") Then v(i) = Replace(v(i), ";", "<br />") 'более удачный для экспорта вариант?
		If InStr(v(i), ";") Then v(i) = "<ul><li>" & Replace(v(i), ";", "</li><li>") & "</li></ul>"
	Next
	ReadCSV = "<tr><td>" & Join(v, "</td><td>") & "</td></tr>"
End Function