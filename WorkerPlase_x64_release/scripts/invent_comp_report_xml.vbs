'Инвентаризация компьютеров средствами WMI и VBScript (11.06.2009)
'Сборка общего HTML-отчета по всем компьютерам из отдельных CSV-файлов
'Подробности - см. http://zheleznov.info/invent_comp.htm
'Автор: Павел Железнов
'Переделано под нужды программы "Учёт рабочих мест": Владимир Криворак

'== НАСТРОЙКИ

Const TITLE = "Инвентаризация компьютеров" 'заголовок отчета и диалоговых окон
Const DATA_DIR = "comp\" 'каталог для сохранения отчетов + "\" в конце
'Const DATA_DIR = "\\SRV\Invent\comp\" 'сетевой ресурс для сохранения отчетов + "\" в конце
Const DATA_EXT = ".csv" 'расширение файлов с данными
Const HEAD_LINE = True 'пропустить первую строку в файле CSV - заголовок
Const REPORT_FILE = "comp_report_%DATE%.xml" 'имя файла для сохранения отчета

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

'== ВЫПОЛНЕНИЕ

'файл отчета
Dim fso, report, rootNode
Set fso = CreateObject("Scripting.FileSystemObject")
report = Replace(REPORT_FILE, "%DATE%", Date)

Set xmlParser = CreateObject("Msxml2.DOMDocument")
'Создание объявления XML
xmlParser.appendChild(xmlParser.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"))
'Создание корневого элемента
Set rootNode = xmlParser.appendChild(xmlParser.createElement("report"))

'прочитать все CSV-файлы
Dim dir, fc, f, row
Set dir = fso.GetFolder(DATA_DIR)
Set fc = dir.Files
For Each f in fc
	ReadCSV(dir.Path & "\" & f.Name)
	'If Right(f.Name, 4) = DATA_EXT Then row = ReadCSV(dir.Path & "\" & f.Name)
	'If Len(row) > 0 Then rep.WriteLine row
Next

'сохранить файл отчета
xmlParser.save(report)
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

'удаление дублирующихся пробелов
Function delDubleSpace(S1)
    Dim S
    S = Trim(S1)
    While InStr(1, S, "  ") <> 0
       S = Left(S, InStr(1, S, "  ") - 1) & Right(S, Len(S) - InStr(1, S, "  "))
    Wend
    delDubleSpace = S
End Function

'извлечь данные из файла CSV
'сформировать и вернуть строку таблицы в формате XHTML
'в случае ошибки вернуть пустую строку
Function ReadCSV(fname)
	Dim tf, s, a, b, c, k, i, ram, hdd
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

	'создаём рабочее место
	Set wpNode = xmlParser.createElement("workplase")
	wpNode.setAttribute "name", v(0)

	'добавляем устройство
	Set deviceNode = wpNode.appendChild(xmlParser.createElement("device"))
	deviceNode.setAttribute "type", "pc"
	If InStr(v(11), ";") Then
		a = Split(v(11), ";")
		ram = 0
		For i = 0 To UBound(a)
			ram = ram + a(i)
		Next
	Else
		ram = v(11)
	End If
	If InStr(v(14), ";") Then
		a = Split(v(14), ";")
		hdd = 0
		For i = 0 To UBound(a)
			hdd = hdd + a(i)
		Next
	Else
		hdd = v(14)
	End If
	v(7) = delDubleSpace(v(7))
	deviceNode.setAttribute "name", v(7) & " " & v(8) & "/" & "'" & v(6) & "'" & "/" & "RAM " & ram & "Мб" & "/" & "HDD " & hdd & "Гб" & "/" & v(17) & " " &v(18)
	
	'добавляем комплектуху
	Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
	subDeviceNode.setAttribute "type", "motherboard"
	subDeviceNode.setAttribute "name", v(5) & " " & v(6)
	subDeviceNode.setAttribute "producer",  v(5)
	'--------------------------------
	If InStr(v(7), ";") Then
		a = Split(v(7), ";")
		b = Split(v(8), ";")
		For i = 0 To UBound(b)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "procesor"
			subDeviceNode.setAttribute "name", a(i) & " " & b(i)
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "procesor"
		subDeviceNode.setAttribute "name", v(7) & " " & v(8)
	End If
	'--------------------------------
	If InStr(v(11), ";") Then
		b = Split(v(11), ";")
		If  v(10) = "-" Then
			For i = 0 To UBound(b)
				a(i) = "-"
			Next
		Else
			a = Split(v(10), ";")
		End If
		For i = 0 To UBound(b)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "ram"
			subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "Мб "
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "ram"
		subDeviceNode.setAttribute "name", v(10) & " " & v(11) & "Мб "
	End If
	'--------------------------------
	If InStr(v(13), ";") Then
		a = Split(v(13), ";")
		b = Split(v(14), ";")
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "hdd"
			subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "Гб "
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "hdd"
		subDeviceNode.setAttribute "name", v(13) & " " & v(14) & "Гб "
	End If
	'--------------------------------
	If InStr(v(16), ";") Then
		a = Split(v(16), ";")
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "cdrom"
			subDeviceNode.setAttribute "name", a(i)
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "cdrom"
		subDeviceNode.setAttribute "name", v(16)
	End If
	'--------------------------------
	If InStr(v(17), ";") Then
		a = Split(v(17), ";")
		If InStr(v(18), ";") Then
			b = Split(v(18), ";")
		End If
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "video"
			If InStr(v(18), ";") Then
				subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "Мб "
			Else
				subDeviceNode.setAttribute "name", a(i) & " " & v(18) & "Мб "
			End If
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "video"
		subDeviceNode.setAttribute "name", v(17) & " " & v(18) & "Мб "
	End If
	'--------------------------------
	If InStr(v(19), ";") Then
		a = Split(v(19), ";")
		If InStr(v(20), ";") Then
			b = Split(v(20), ";")
		End If
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "network"
			subDeviceNode.setAttribute "name", a(i)
			If InStr(v(20), ";") AND i <= UBound(b) Then
				subDeviceNode.setAttribute "mac", b(i)
			End If
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "network"
		subDeviceNode.setAttribute "name", v(19)
		if Len(v(20)) > 0 Then
			subDeviceNode.setAttribute "mac", v(20)
		End If
	End If
	'--------------------------------
	If InStr(v(21), ";") Then
		a = Split(v(21), ";")
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "audio"
			subDeviceNode.setAttribute "name", a(i)
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "audio"
		subDeviceNode.setAttribute "name", v(21)
	End If
	
	'Добавляем операционную систему
	Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("license"))
	subDeviceNode.setAttribute "name", v(3) & " " & v(4)
	
	rootNode.appendChild(wpNode)
End Function