'�������������� ����������� ���������� WMI � VBScript (29.07.2009)
'������ ��������� � ��������� ���� CSV ��� ����������� ������ ������ ������
'����������� - ��. http://zheleznov.info/invent_comp.htm
'�����: ����� ��������

'== ���������

'����� ������
Const SILENT = False '����� ����� ��������, ����� ��������� ��� ����������
'Const SILENT = True '����� ������ � ��������� ���������� ��� ������ ��������

'��� ��������� �����
Const DATA_DIR = "comp\" '��������� ������� + "\" � �����
'Const DATA_DIR = "\\filesrv\�����\report\" '������� ������ + "\" � �����

'������
Const TITLE = "�������������� �����������" '��������� ���������� ����
Const DATA_EXT = ".csv" '���������� ����� ������
Const HEAD_LINE = True '�������� ��������� � ������ ������ CSV-�����

'�� ��������� ������ ��������
'���������������� �� ����� �������
On Error Resume Next

'== ����������

'������ ��� ������� � �������� �������
Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")

'������ WMI
Dim wmio

'���� ������
Dim tf

'������ ��� ���������� ����������
Dim nwo, comp
Set nwo = CreateObject("WScript.Network")
comp = LCase(nwo.ComputerName)

'��������� ��� ���������� ����������
If Not SILENT Then
	comp = InputBox("������� ��� ����������:", TITLE, comp)
	'��������� ����������� ����������
	If Unavailable(comp) Then
		MsgBox "��������� ����������:" & vbCrLf & comp, vbExclamation, TITLE
		comp = ""
	End If
End If

'�������� ��������������
If Len(comp) > 0 Then InventComp(comp)

'���� ������
If Len(Err.Description) > 0 Then _
	If Not SILENT Then MsgBox comp & vbCrLf & "������:" & vbCrLf & Err.Description, vbExclamation, TITLE

'== ������������

'�������������� ����������, ��������� ������� ������ ��� IP-�������
'���������� ������ � ��������� ������
Sub InventComp(compname)

	Set wmio = GetObject("WinMgmts:{impersonationLevel=impersonate}!\\" & compname & "\Root\CIMV2")

	'��������� WMI-������ �������������� �� �� ���� ������� Windows
	Dim build
	build = BuildVersion()

	'���� ������
	Set tf = fso.CreateTextFile(DATA_DIR & compname & DATA_EXT, True, True)

	'������ ������ - ���������
	If HEAD_LINE Then tf.WriteLine "������ ������;��������;����� ����������;��������"

	'���� ��������
	tf.WriteLine "���������;���� ��������;1;" & Now

	Log "Win32_ComputerSystemProduct", _
		"UUID", "", _
		"���������", _
		"UUID"

	Log "Win32_ComputerSystem", _
		"Name,Domain,PrimaryOwnerName,UserName,TotalPhysicalMemory", "", _
		"���������", _
		"������� ���,�����,��������,������� ������������,����� ������ (��)"

	Log "Win32_OperatingSystem", _
		"Caption,Version,CSDVersion,Description,RegisteredUser,SerialNumber,Organization,InstallDate", "", _
		"������������ �������", _
		"������������,������,����������,��������,������������������ ������������,�������� �����,�����������,���� ���������"

	Log "Win32_BaseBoard", _
		"Manufacturer,Product,Version,SerialNumber", "", _
		"����������� �����", _
		"�������������,������������,������,�������� �����"

	Log "Win32_BIOS", _
		"Manufacturer,Name,SMBIOSBIOSVersion,SerialNumber", "", _
		"BIOS", _
		"�������������,������������,������,�������� �����"

	'�� ������������ Core 2 � XP SP2, ��. http://support.microsoft.com/kb/953955
	Log "Win32_Processor", _
		"Name,Caption,CurrentClockSpeed,ExtClock,L2CacheSize,SocketDesignation,UniqueId", "", _
		"���������", _
		"������������,��������,������� (���),������� FSB (���),������ L2-���� (��),������,UID"

	Log "Win32_PhysicalMemory", _
		"MemoryType,Capacity,Speed,DeviceLocator", "", _
		"������ ������", _
		"���,������ (��),�������,����������"

	'������������ USB-�����
	Log "Win32_DiskDrive", _
		"Model,Size,InterfaceType", "InterfaceType <> 'USB'", _
		"����", _
		"������������,������ (��),���������"

	'������ ��������� �����
	'������������ USB-�����, ������ ������� ������ NULL
	Log "Win32_LogicalDisk", _
		"Name,FileSystem,Size,FreeSpace,VolumeSerialNumber", "DriveType = 3 AND Size IS NOT NULL", _
		"���������� ����", _
		"������������,�������� �������,������ (��),�������� (��),�������� �����"

	Log "Win32_CDROMDrive", _
		"Name", "", _
		"CD-������", _
		"������������"

	'������ ��� XP/2003 � ����
	'������������ "��������", ������� � �������� ����� "Secondary"
	If build >= 2600 Then
		Log "Win32_VideoController", _
		"Name,AdapterRAM,VideoProcessor,VideoModeDescription,DriverDate,DriverVersion", "NOT (Name LIKE '%Secondary')", _
		"���������������", _
		"������������,����� ������ (��),��������������,����� ������,���� ��������,������ ��������"
	Else '��� Windows 2000
		Log "Win32_VideoController", _
		"Name,AdapterRAM,VideoProcessor,VideoModeDescription,DriverDate,DriverVersion", "", _
		"���������������", _
		"������������,����� ������ (��),��������������,����� ������,���� ��������,������ ��������"
	End If

	'������ ��� XP/2003 � ����
	'������������ ����������� ������� ��������, � ��� ����� ���������
	'������������ ����������� �������� VMware
	If build >= 2600 Then
		Log "Win32_NetworkAdapter", _
		"Name,AdapterType,PermanentAddress,MACAddress", "NetConnectionStatus > 0 AND NOT (Name LIKE 'VMware%')", _
		"������� �������", _
		"������������,���,IP-�����,MAC-�����"
	Else '��� Windows 2000
		Log "Win32_NetworkAdapter", _
		"Name,PermanentAddress,MACAddress", "", _
		"������� �������", _
		"������������,IP-�����,MAC-�����"
	End If

	Log "Win32_SoundDevice", _
		"Name", "", _
		"�������� ����������", _
		"������������"

	Log "Win32_SCSIController", _
		"Name", "", _
		"SCSI ����������", _
		"������������"

	'������ ��� XP/2003 � ����
	'������������ ������� ��������
	'������� "Local = True Or Network = False" ������������ ��� �����-��������, ������� ����������� ����
	If build >= 2600 Then
		Log "Win32_Printer", _
		"Name,PortName,ShareName", "(Local = True OR Network = False) AND (PortName LIKE '%USB%' OR PortName LIKE '%LPT%')", _
		"�������", _
		"������������,����,������� ���"
	End If

	Log "Win32_PortConnector", _
		"ExternalReferenceDesignator,InternalReferenceDesignator", "", _
		"������ �����", _
		"�������,����������"

	Log "Win32_Keyboard", _
		"Name,Description", "", _
		"����������", _
		"������������,��������"

	Log "Win32_PointingDevice", _
		"Name", "", _
		"����", _
		"������������"

	'������� ���� 
	tf.Close
	If Not SILENT Then MsgBox "����� �������� � ����:" & vbCrLf & DATA_DIR & compname & DATA_EXT, vbInformation, TITLE

End Sub

'��������� WQL-������, ��������� � �������� ������ � CSV-����
'������� ���������:
'from - ����� WMI
'sel - �������� WMI, ����� �������
'where - ������� ������ ��� ������ ������
'sect - ��������������� ������ ������
'param - ��������������� ��������� ������ ������ ������, ����� �������
'��� ����������� � ������� ��������, ����� �� ������� � �������
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

	num = 1 '����� ����������
	For Each item In cls
		For i = 0 To UBound(props)

			'����� ��������
			Set prop = item.Properties_(props(i))
			value = prop.Value

			'��� �������� �� Null ��������� ����� � �������
			If IsNull(value) Then
				value = ""

			'���� ��� ������ - ������, ������� � ������
			ElseIf IsArray(value) Then
				value = Join(value,",")

			'���� ������� ������� ������� ���������, ��������� ��������
			ElseIf Right(names(i), 4) = "(��)" Then
				value = CStr(Round(value / 1024 ^ 2))
			ElseIf Right(names(i), 4) = "(��)" Then
				value = CStr(Round(value / 1024 ^ 3))

			'���� ��� ������ - ����, ������������� � �������� ���
			ElseIf prop.CIMType = 101 Then
				value = ReadableDate(value)
			End If

			'������� � ���� �������� ��������, �������� ���������� ";"
			value = Trim(Replace(value, ";", "_"))
			If Len(value) > 0 Then tf.WriteLine sect & ";" & names(i) & ";" & num & ";" & value

		Next 'i

		'������� � ���������� ����������
		num = num + 1
	Next 'item

End Sub

'�������������� ���� ������� DMTF � �������� ��� (��.��.����)
'http://msdn.microsoft.com/en-us/library/aa389802.aspx
Function ReadableDate(str)
'������ ���������� � Windows 2000, ������� ��. �����
'	Dim dto
'	Set dto = CreateObject("WbemScripting.SWbemDateTime")
'	dto.Value = str
'	ReadableDate = dto.GetVarDate(True)
	ReadableDate = Mid(str, 7, 2) & "." & Mid(str, 5, 2) & "." & Left(str, 4)
End Function

'������ ������ (����) WMI-�������
'������� ����� �����
Function BuildVersion()
	Dim cls, item
	Set cls = wmio.ExecQuery("Select BuildVersion From Win32_WMISetting")
	For Each item In cls
		BuildVersion = CInt(Left(item.BuildVersion, 4))
	Next
End Function

'��������� ����������� ���������� � ����
'������� True, ���� ����� ����������
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