'Инвентаризация компьютеров средствами WMI и VBScript (29.07.2009)
'Данные выводятся в отдельный файл CSV для последующей сборки общего отчета
'Подробности - см. http://zheleznov.info/invent_comp.htm
'Автор: Павел Железнов

'== НАСТРОЙКИ

'режим работы
Const SILENT = False 'тихий режим отключен, будет запрошено имя компьютера
'Const SILENT = True 'режим отчета о локальном компьютере без вывода диалогов

'где сохранять отчет
Const DATA_DIR = "comp\" 'локальный каталог + "\" в конце
'Const DATA_DIR = "\\filesrv\Обмен\report\" 'сетевой ресурс + "\" в конце

'прочее
Const TITLE = "Инвентаризация компьютеров" 'заголовок диалоговых окон
Const DATA_EXT = ".csv" 'расширение файла отчета
Const HEAD_LINE = True 'выводить заголовки в первой строке CSV-файла

'не завершать скрипт аварийно
'закомментировать на время отладки
On Error Resume Next

'== ВЫПОЛНЕНИЕ

'объект для доступа к файловой системе
Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")

'объект WMI
Dim wmio

'файл отчета
Dim tf

'узнать имя локального компьютера
Dim nwo, comp
Set nwo = CreateObject("WScript.Network")
comp = LCase(nwo.ComputerName)

'запросить имя удаленного компьютера
If Not SILENT Then
	comp = InputBox("Введите имя компьютера:", TITLE, comp)
	'проверить доступность компьютера
	If Unavailable(comp) Then
		MsgBox "Компьютер недоступен:" & vbCrLf & comp, vbExclamation, TITLE
		comp = ""
	End If
End If

'провести инвентаризацию
If Len(comp) > 0 Then InventComp(comp)

'если ошибка
If Len(Err.Description) > 0 Then _
	If Not SILENT Then MsgBox comp & vbCrLf & "Ошибка:" & vbCrLf & Err.Description, vbExclamation, TITLE

'== ПОДПРОГРАММЫ

'инвентаризация компьютера, заданного сетевым именем или IP-адресом
'сохранение отчета с указанным именем
Sub InventComp(compname)

	Set wmio = GetObject("WinMgmts:{impersonationLevel=impersonate}!\\" & compname & "\Root\CIMV2")

	'некоторые WMI-классы поддерживаются не во всех версиях Windows
	Dim build
	build = BuildVersion()

	'файл отчета
	Set tf = fso.CreateTextFile(DATA_DIR & compname & DATA_EXT, True, True)

	'первая строка - заголовки
	If HEAD_LINE Then tf.WriteLine "Секция отчета;Параметр;Номер экземпляра;Значение"

	'дата проверки
	tf.WriteLine "Компьютер;Дата проверки;1;" & Now

	Log "Win32_ComputerSystemProduct", _
		"UUID", "", _
		"Компьютер", _
		"UUID"

	Log "Win32_ComputerSystem", _
		"Name,Domain,PrimaryOwnerName,UserName,TotalPhysicalMemory", "", _
		"Компьютер", _
		"Сетевое имя,Домен,Владелец,Текущий пользователь,Объем памяти (Мб)"

	Log "Win32_OperatingSystem", _
		"Caption,Version,CSDVersion,Description,RegisteredUser,SerialNumber,Organization,InstallDate", "", _
		"Операционная система", _
		"Наименование,Версия,Обновление,Описание,Зарегистрированный пользователь,Серийный номер,Организация,Дата установки"

	Log "Win32_BaseBoard", _
		"Manufacturer,Product,Version,SerialNumber", "", _
		"Материнская плата", _
		"Производитель,Наименование,Версия,Серийный номер"

	Log "Win32_BIOS", _
		"Manufacturer,Name,SMBIOSBIOSVersion,SerialNumber", "", _
		"BIOS", _
		"Производитель,Наименование,Версия,Серийный номер"

	'не определяется Core 2 в XP SP2, см. http://support.microsoft.com/kb/953955
	Log "Win32_Processor", _
		"Name,Caption,CurrentClockSpeed,ExtClock,L2CacheSize,SocketDesignation,UniqueId", "", _
		"Процессор", _
		"Наименование,Описание,Частота (МГц),Частота FSB (МГц),Размер L2-кеша (кб),Разъем,UID"

	Log "Win32_PhysicalMemory", _
		"MemoryType,Capacity,Speed,DeviceLocator", "", _
		"Модуль памяти", _
		"Тип,Размер (Мб),Частота,Размещение"

	'пропускаются USB-диски
	Log "Win32_DiskDrive", _
		"Model,Size,InterfaceType", "InterfaceType <> 'USB'", _
		"Диск", _
		"Наименование,Размер (Гб),Интерфейс"

	'только локальные диски
	'пропускаются USB-диски, размер которых обычно NULL
	Log "Win32_LogicalDisk", _
		"Name,FileSystem,Size,FreeSpace,VolumeSerialNumber", "DriveType = 3 AND Size IS NOT NULL", _
		"Логический диск", _
		"Наименование,Файловая система,Размер (Гб),Свободно (Гб),Серийный номер"

	Log "Win32_CDROMDrive", _
		"Name", "", _
		"CD-привод", _
		"Наименование"

	'только для XP/2003 и выше
	'пропускаются "двойники", имеющие в названии слово "Secondary"
	If build >= 2600 Then
		Log "Win32_VideoController", _
		"Name,AdapterRAM,VideoProcessor,VideoModeDescription,DriverDate,DriverVersion", "NOT (Name LIKE '%Secondary')", _
		"Видеоконтроллер", _
		"Наименование,Объем памяти (Мб),Видеопроцессор,Режим работы,Дата драйвера,Версия драйвера"
	Else 'для Windows 2000
		Log "Win32_VideoController", _
		"Name,AdapterRAM,VideoProcessor,VideoModeDescription,DriverDate,DriverVersion", "", _
		"Видеоконтроллер", _
		"Наименование,Объем памяти (Мб),Видеопроцессор,Режим работы,Дата драйвера,Версия драйвера"
	End If

	'только для XP/2003 и выше
	'пропускаются отключенные сетевые адаптеры, в том числе минипорты
	'пропускаются виртуальные адаптеры VMware
	If build >= 2600 Then
		Log "Win32_NetworkAdapter", _
		"Name,AdapterType,PermanentAddress,MACAddress", "NetConnectionStatus > 0 AND NOT (Name LIKE 'VMware%')", _
		"Сетевой адаптер", _
		"Наименование,Тип,IP-адрес,MAC-адрес"
	Else 'для Windows 2000
		Log "Win32_NetworkAdapter", _
		"Name,PermanentAddress,MACAddress", "", _
		"Сетевой адаптер", _
		"Наименование,IP-адрес,MAC-адрес"
	End If

	Log "Win32_SoundDevice", _
		"Name", "", _
		"Звуковое устройство", _
		"Наименование"

	Log "Win32_SCSIController", _
		"Name", "", _
		"SCSI контроллер", _
		"Наименование"

	'только для XP/2003 и выше
	'пропускаются сетевые принтеры
	'условия "Local = True Or Network = False" недостаточно для принт-серверов, поэтому проверяется порт
	If build >= 2600 Then
		Log "Win32_Printer", _
		"Name,PortName,ShareName", "(Local = True OR Network = False) AND (PortName LIKE '%USB%' OR PortName LIKE '%LPT%')", _
		"Принтер", _
		"Наименование,Порт,Сетевое имя"
	End If

	Log "Win32_PortConnector", _
		"ExternalReferenceDesignator,InternalReferenceDesignator", "", _
		"Разъем порта", _
		"Внешний,Внутренний"

	Log "Win32_Keyboard", _
		"Name,Description", "", _
		"Клавиатура", _
		"Наименование,Описание"

	Log "Win32_PointingDevice", _
		"Name", "", _
		"Мышь", _
		"Наименование"

	'закрыть файл 
	tf.Close
	If Not SILENT Then MsgBox "Отчет сохранен в файл:" & vbCrLf & DATA_DIR & compname & DATA_EXT, vbInformation, TITLE

End Sub

'составить WQL-запрос, выполнить и записать строку в CSV-файл
'входные параметры:
'from - класс WMI
'sel - свойства WMI, через запятую
'where - условие отбора или пустая строка
'sect - соответствующая секция отчета
'param - соответствующие параметры внутри секции отчета, через запятую
'для отображения в кратных единицах, нужно их указать в скобках
Sub Log(from, sel, where, sect, param)

	Const RETURN_IMMEDIATELY = 16
	Const FORWARD_ONLY = 32

	Dim query, cls, item, prop
	query = "Select " & sel & " From " & from

	If Len(where) > 0 Then query = query & " Where " & where
	Set cls = wmio.ExecQuery(query,, RETURN_IMMEDIATELY + FORWARD_ONLY)

	Dim props, names, num, value
	props = Split(sel, ",")
	names = Split(param, ",")

	num = 1 'номер экземпляра
	For Each item In cls
		For i = 0 To UBound(props)

			'взять значение
			Set prop = item.Properties_(props(i))
			value = prop.Value

			'без проверки на Null возможнен вылет с ошибкой
			If IsNull(value) Then
				value = ""

			'если тип данных - массив, собрать в строку
			ElseIf IsArray(value) Then
				value = Join(value,",")

			'если указана кратная единица измерения, перевести значение
			ElseIf Right(names(i), 4) = "(Мб)" Then
				value = CStr(Round(value / 1024 ^ 2))
			ElseIf Right(names(i), 4) = "(Гб)" Then
				value = CStr(Round(value / 1024 ^ 3))

			'если тип данных - дата, преобразовать в читаемый вид
			ElseIf prop.CIMType = 101 Then
				value = ReadableDate(value)
			End If

			'вывести в файл непустое значение, заменить спецсимвол ";"
			value = Trim(Replace(value, ";", "_"))
			If Len(value) > 0 Then tf.WriteLine sect & ";" & names(i) & ";" & num & ";" & value

		Next 'i

		'перейти к следующему экземпляру
		num = num + 1
	Next 'item

End Sub

'преобразование даты формата DMTF в читаемый вид (ДД.ММ.ГГГГ)
'http://msdn.microsoft.com/en-us/library/aa389802.aspx
Function ReadableDate(str)
'объект недоступен в Windows 2000, поэтому см. далее
'	Dim dto
'	Set dto = CreateObject("WbemScripting.SWbemDateTime")
'	dto.Value = str
'	ReadableDate = dto.GetVarDate(True)
	ReadableDate = Mid(str, 7, 2) & "." & Mid(str, 5, 2) & "." & Left(str, 4)
End Function

'узнать версию (билд) WMI-сервера
'вернуть целое число
Function BuildVersion()
	Dim cls, item
	Set cls = wmio.ExecQuery("Select BuildVersion From Win32_WMISetting")
	For Each item In cls
		BuildVersion = CInt(Left(item.BuildVersion, 4))
	Next
End Function

'проверить доступность компьютера в сети
'вернуть True, если адрес недоступен
Function Unavailable(addr)
	Dim wmio, ping, p
	Set wmio = GetObject("WinMgmts:{impersonationLevel=impersonate}")
	Set ping = wmio.ExecQuery("SELECT StatusCode FROM Win32_PingStatus WHERE Address = '" & addr & "'")
	For Each p In ping
		If IsNull(p.StatusCode) Then
			Unavailable = True
		Else
			Unavailable = (p.StatusCode <> 0)
		End If
	Next
End Function