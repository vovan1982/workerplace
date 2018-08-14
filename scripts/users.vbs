'== НАСТРОЙКИ

Const TITLE = "Импорт пользователей" 'заголовок отчета и диалоговых окон
Const DATA_DIR = "users\" 'каталог для сохранения отчетов + "\" в конце
'Const DATA_DIR = "\\SRV\Invent\comp\" 'сетевой ресурс для сохранения отчетов + "\" в конце
Const DATA_EXT = ".csv" 'расширение файлов с данными
Const HEAD_LINE = True 'пропустить первую строку в файле CSV - заголовок
Const REPORT_FILE = "users_%DATE%.xml" 'имя файла для сохранения отчета

'== ВЫПОЛНЕНИЕ

Class Class_User
	Dim lastname
	Dim firstname
	Dim middlename
	Dim displayname
	Dim post
	Dim email
	Dim additionalemail
	Dim login
End Class

Class Class_Organization
	Dim name
	Dim user()
	Public Sub redimenize(v)
		ReDim Preserve user(v)
	End Sub
End Class

Dim Organization()

'файл отчета
Dim fso, users, rootNode
Set fso = CreateObject("Scripting.FileSystemObject")
users = Replace(REPORT_FILE, "%DATE%", Date)

Set xmlParser = CreateObject("Msxml2.DOMDocument")
'Создание объявления XML
xmlParser.appendChild(xmlParser.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"))
'Создание корневого элемента
Set rootNode = xmlParser.appendChild(xmlParser.createElement("users"))

'прочитать все CSV-файлы
Dim dir, fc, f, row
Set dir = fso.GetFolder(DATA_DIR)
Set fc = dir.Files
For Each f in fc
	ReadCSV(dir.Path & "\" & f.Name)
Next

'сохранить файл отчета
xmlParser.save(users)
MsgBox "Отчет сохранен в файл:" & vbCrLf & users, vbInformation, TITLE

'== ПОДПРОГРАММЫ

'извлечь данные из файла CSV
'сформировать и вернуть строку таблицы в формате XHTML
'в случае ошибки вернуть пустую строку
Function ReadCSV(fname)
	Dim tf, s, a, b, c, i, j, k
	
	Set tf = fso.OpenTextFile(fname, 1, False, True)
	If HEAD_LINE Then tf.ReadLine 'пропустить первую строку
	i = 0
	b = 0
	ReDim Preserve Organization(i)
	Set Organization(i) = New Class_Organization
	Organization(i).redimenize(0)
	Set Organization(i).user(0) = New Class_User
	
	Do Until tf.AtEndOfStream
		s = tf.ReadLine
		a = Split(s, ";")
		findWp = 0
		
		If b > 0 Then
			For j = 0 To UBound(Organization)
				If Organization(j).name = a(0) Then
					i = j
					findWp = 1
					Exit For
				End If
			Next
		End If
		
		if findWp = 0 Then
			If b > 0 Then
				ReDim Preserve Organization(UBound(Organization)+1)
			End If
			i = UBound(Organization)
			Set Organization(i) = New Class_Organization
			Organization(i).name = a(0)
			Organization(i).redimenize(0)
			k = 0
		Else
			Organization(i).redimenize(UBound(Organization(i).user)+1)
			k = UBound(Organization(i).user)
		End If
		
		Set Organization(i).user(k) = New Class_User
		Organization(i).user(k).lastname = a(1)
		Organization(i).user(k).firstname = a(2)
		Organization(i).user(k).middlename = a(3)
		Organization(i).user(k).displayname = a(4)
		Organization(i).user(k).post = a(5)
		Organization(i).user(k).email = a(6)
		Organization(i).user(k).additionalemail = a(7)
		Organization(i).user(k).login = a(8)
		b = b + 1
	Loop
	tf.Close

	For i = 0 To UBound(Organization)
		'создаём организацию
		Set wpNode = xmlParser.createElement("organization")
		wpNode.setAttribute "name", Organization(i).name

		'добавляем пользователя
		For k = 0 To UBound(Organization(i).user)
			Set userNode = wpNode.appendChild(xmlParser.createElement("user"))
			userNode.setAttribute "lastname", Organization(i).user(k).lastname
			userNode.setAttribute "firstname", Organization(i).user(k).firstname
			userNode.setAttribute "middlename", Organization(i).user(k).middlename
			userNode.setAttribute "displayname", Organization(i).user(k).displayname
			userNode.setAttribute "post", Organization(i).user(k).post
			userNode.setAttribute "email", Organization(i).user(k).email
			userNode.setAttribute "additionalemail", Organization(i).user(k).additionalemail
			userNode.setAttribute "login", Organization(i).user(k).login
		Next
		
		rootNode.appendChild(wpNode)
	Next
End Function