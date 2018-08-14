'�������������� ����������� ���������� WMI � VBScript (11.06.2009)
'������ ������ HTML-������ �� ���� ����������� �� ��������� CSV-������
'����������� - ��. http://zheleznov.info/invent_comp.htm
'�����: ����� ��������
'���������� ��� ����� ��������� "���� ������� ����": �������� ��������

'== ���������

Const TITLE = "�������������� �����������" '��������� ������ � ���������� ����
Const DATA_DIR = "comp\" '������� ��� ���������� ������� + "\" � �����
'Const DATA_DIR = "\\SRV\Invent\comp\" '������� ������ ��� ���������� ������� + "\" � �����
Const DATA_EXT = ".csv" '���������� ������ � �������
Const HEAD_LINE = True '���������� ������ ������ � ����� CSV - ���������
Const REPORT_FILE = "comp_report_%DATE%.xml" '��� ����� ��� ���������� ������

'����������, ������� � �������� �������� ������
'�������� ������ ��������������� ������ ���� ����� CSV �����!
Dim col(22) '<-- �� ������ ��������� ������� ������!
col(0) = "���������;������� ���"
col(1) = "���������;UUID"
col(2) = "���������;������� ������������"
col(3) = "������������ �������;������������"
col(4) = "������������ �������;����������"
col(5) = "����������� �����;�������������"
col(6) = "����������� �����;������������"
col(7) = "���������;������������"
col(8) = "���������;������� (���)"
col(9) = "���������;����� ������ (��)"
col(10) = "������ ������;���"
col(11) = "������ ������;������ (��)"
col(12) = "������ ������;�������"
col(13) = "����;������������"
col(14) = "����;������ (��)"
col(15) = "����;���������"
col(16) = "CD-������;������������"
col(17) = "���������������;������������"
col(18) = "���������������;����� ������ (��)"
col(19) = "������� �������;������������"
col(20) = "������� �������;MAC-�����"
col(21) = "�������� ����������;������������"
col(22) = "�������;������������"

'== ����������

'���� ������
Dim fso, report, rootNode
Set fso = CreateObject("Scripting.FileSystemObject")
report = Replace(REPORT_FILE, "%DATE%", Date)

Set xmlParser = CreateObject("Msxml2.DOMDocument")
'�������� ���������� XML
xmlParser.appendChild(xmlParser.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"))
'�������� ��������� ��������
Set rootNode = xmlParser.appendChild(xmlParser.createElement("report"))

'��������� ��� CSV-�����
Dim dir, fc, f, row
Set dir = fso.GetFolder(DATA_DIR)
Set fc = dir.Files
For Each f in fc
	ReadCSV(dir.Path & "\" & f.Name)
	'If Right(f.Name, 4) = DATA_EXT Then row = ReadCSV(dir.Path & "\" & f.Name)
	'If Len(row) > 0 Then rep.WriteLine row
Next

'��������� ���� ������
xmlParser.save(report)
MsgBox "����� �������� � ����:" & vbCrLf & report, vbInformation, TITLE

'== ������������

'���������� ������ �������� ������� �� ��� ��������
'���� �������� �� �������, ������� -1
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

'���� ����������� ������
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

'�������� ������������� ��������
Function delDubleSpace(S1)
    Dim S
    S = Trim(S1)
    While InStr(1, S, "  ") <> 0
       S = Left(S, InStr(1, S, "  ") - 1) & Right(S, Len(S) - InStr(1, S, "  "))
    Wend
    delDubleSpace = S
End Function

'������� ������ �� ����� CSV
'������������ � ������� ������ ������� � ������� XHTML
'� ������ ������ ������� ������ ������
Function ReadCSV(fname)
	Dim tf, s, a, b, c, k, i, ram, hdd
	Dim v()
	ReDim v(UBound(col))
	'�������� �� ��������� - "-"
	For i = 0 To UBound(v)
		v(i) = "-"
	Next
	Set tf = fso.OpenTextFile(fname, 1, False, True)
	If HEAD_LINE Then tf.ReadLine '���������� ������ ������
	Do Until tf.AtEndOfStream
		s = tf.ReadLine
		a = Split(s, ";")
		k = a(0) & ";" & a(1)
		i = IndexCol(k)
		If i > -1 Then
			If a(2) > 1 Then '��������� ����������� ����������� ";"
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

	'������ ������� �����
	Set wpNode = xmlParser.createElement("workplase")
	wpNode.setAttribute "name", v(0)

	'��������� ����������
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
	deviceNode.setAttribute "name", v(7) & " " & v(8) & "/" & "'" & v(6) & "'" & "/" & "RAM " & ram & "��" & "/" & "HDD " & hdd & "��" & "/" & v(17) & " " &v(18)
	
	'��������� �����������
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
			subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "�� "
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "ram"
		subDeviceNode.setAttribute "name", v(10) & " " & v(11) & "�� "
	End If
	'--------------------------------
	If InStr(v(13), ";") Then
		a = Split(v(13), ";")
		b = Split(v(14), ";")
		For i = 0 To UBound(a)
			Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
			subDeviceNode.setAttribute "type", "hdd"
			subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "�� "
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "hdd"
		subDeviceNode.setAttribute "name", v(13) & " " & v(14) & "�� "
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
				subDeviceNode.setAttribute "name", a(i) & " " & b(i) & "�� "
			Else
				subDeviceNode.setAttribute "name", a(i) & " " & v(18) & "�� "
			End If
		Next
	Else
		Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("subdevice"))
		subDeviceNode.setAttribute "type", "video"
		subDeviceNode.setAttribute "name", v(17) & " " & v(18) & "�� "
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
	
	'��������� ������������ �������
	Set subDeviceNode = deviceNode.appendChild(xmlParser.createElement("license"))
	subDeviceNode.setAttribute "name", v(3) & " " & v(4)
	
	rootNode.appendChild(wpNode)
End Function