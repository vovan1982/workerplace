'== ���������

Const TITLE = "�������������� ���������" '��������� ������ � ���������� ����
Const DATA_DIR = "comp\" '������� ��� ���������� ������� + "\" � �����
'Const DATA_DIR = "\\SRV\Invent\comp\" '������� ������ ��� ���������� ������� + "\" � �����
Const DATA_EXT = ".csv" '���������� ������ � �������
Const HEAD_LINE = True '���������� ������ ������ � ����� CSV - ���������
Const REPORT_FILE = "comp_report_%DATE%.xml" '��� ����� ��� ���������� ������

'== ����������

Class Class_Device
	Dim types
	Dim name
	Dim producer
	Dim serialN
	Dim invN
End Class

Class Class_WorkPlace
	Dim name
	Dim device()
	Public Sub redimenize(v)
		ReDim Preserve device(v)
	End Sub
End Class

Dim WorkPlace()

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
	Dim tf, s, a, b, c, i, j, k
	
	Set tf = fso.OpenTextFile(fname, 1, False, True)
	If HEAD_LINE Then tf.ReadLine '���������� ������ ������
	i = 0
	b = 0
	ReDim Preserve WorkPlace(i)
	Set WorkPlace(i) = New Class_WorkPlace
	WorkPlace(i).redimenize(0)
	Set WorkPlace(i).device(0) = New Class_Device
	
	Do Until tf.AtEndOfStream
		s = tf.ReadLine
		a = Split(s, ";")
		findWp = 0
		
		If b > 0 Then
			For j = 0 To UBound(WorkPlace)
				If WorkPlace(j).name = a(0) Then
					i = j
					findWp = 1
					Exit For
				End If
			Next
		End If
		
		if findWp = 0 Then
			If b > 0 Then
				ReDim Preserve WorkPlace(UBound(WorkPlace)+1)
			End If
			i = UBound(WorkPlace)
			Set WorkPlace(i) = New Class_WorkPlace
			WorkPlace(i).name = a(0)
			WorkPlace(i).redimenize(0)
			k = 0
		Else
			WorkPlace(i).redimenize(UBound(WorkPlace(i).device)+1)
			k = UBound(WorkPlace(i).device)
		End If
		
		Set WorkPlace(i).device(k) = New Class_Device
		WorkPlace(i).device(k).types = a(1)
		WorkPlace(i).device(k).name = a(2)
		WorkPlace(i).device(k).producer = a(3)
		WorkPlace(i).device(k).serialN = a(4)
		WorkPlace(i).device(k).invN = a(5)
		b = b + 1
	Loop
	tf.Close

	For i = 0 To UBound(WorkPlace)
		'������ ������� �����
		Set wpNode = xmlParser.createElement("workplase")
		wpNode.setAttribute "name", WorkPlace(i).name

		'��������� ����������
		For k = 0 To UBound(WorkPlace(i).device)
			Set deviceNode = wpNode.appendChild(xmlParser.createElement("device"))
			deviceNode.setAttribute "type", WorkPlace(i).device(k).types
			deviceNode.setAttribute "name", WorkPlace(i).device(k).name
			deviceNode.setAttribute "producer", WorkPlace(i).device(k).producer
			deviceNode.setAttribute "serialN", WorkPlace(i).device(k).serialN
			deviceNode.setAttribute "invN", WorkPlace(i).device(k).invN
		Next
		
		rootNode.appendChild(wpNode)
	Next
End Function