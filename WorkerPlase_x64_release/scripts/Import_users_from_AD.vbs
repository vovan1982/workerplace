'***************************************************************** 
'* Имя: Import_users_from_AD.vbs                                 * 
'* Язык: VBScript                                                * 
'* Назначение: Импорт пользователей из Active Directory в .csv   *
'*****************************************************************
on error resume next
Dim a, domain, lenStr, displayName
Const strResFile = "users\AdUsers.csv" ' файл с импортированными пользователями
Set objRoot = GetObject("LDAP://RootDSE")
'strDomName = "DC=Example,DC=com"
strDomName = objRoot.Get("DefaultNamingContext") ' домен поумолчанию
Set objRoot = Nothing
strAttributes = "givenName,sn,cn,mail,sAMAccountName" ' поля для выборки из АД
arrCmdText = Array("<LDAP://" & strDomName & ">;(&(objectCategory=Person)(objectClass=User));" & strAttributes & ";Subtree") ' запрос к АД
arrCapLines = Array("Организация;Фамилия;Имя;Отчество;Отображаемое Имя;Должность;Эл. Адрес;Дополнительный Эл. Адрес;Логин входа в систему") ' заголовок файла импорта

a = Split(strDomName, ",")
domain = Right(a(0),Len(a(0)) - 3)
Set objConnection = CreateObject("ADODB.Connection")
objConnection.Provider = "ADsDSOObject"
objConnection.Open "Active Directory Provider"
Set objCommand = CreateObject("ADODB.Command")
Set objCommand.ActiveConnection = objConnection
objCommand.Properties("Page Size") = 1000
objCommand.Properties("Timeout") = 30
objCommand.Properties("Sort On") = "cn"
Set objFS = CreateObject("Scripting.FileSystemObject")
Set objFile = objFS.CreateTextFile(strResFile, True, True)

For i = LBound(arrCmdText) To UBound(arrCmdText)
    objCommand.CommandText = arrCmdText(i)
    Set objRSet = objCommand.Execute
    If objRSet.RecordCount > 0 Then
        objFile.WriteLine(arrCapLines(i))
        objRSet.MoveFirst
        Do Until objRSet.EOF
			displayName = ""
			If Len(objRSet.Fields("sn").Value) > 0 Then
				displayName = objRSet.Fields("sn").Value & "  " & Left(objRSet.Fields("givenName").Value,1) & "."
			Else
				displayName = objRSet.Fields("cn").Value
			End If
			objFile.WriteLine(domain & ";" & objRSet.Fields("sn").Value & ";" & objRSet.Fields("givenName").Value & ";;" & displayName & ";" & "import" & ";" & objRSet.Fields("mail").Value & ";;" & objRSet.Fields("sAMAccountName").Value)
			objRSet.MoveNext
        Loop
    End If
    If i < UBound(arrCmdText) Then objFile.WriteLine(vbCrLf)
Next

objFile.Close
Set objFile = Nothing
Set objFS = Nothing
Set objRSet = Nothing
Set objCommand = Nothing
Set objConnection = Nothing
WScript.Echo "Готово."