use "GunzDB"  

go

/* �̿��� ���� ���(UserGrade) */
INSERT INTO UserGrade (UGradeID, Name) Values (0, '���� ����')
INSERT INTO UserGrade (UGradeID, Name) Values (1, '���� ����')
INSERT INTO UserGrade (UGradeID, Name) Values (253, 'Blocked')
INSERT INTO UserGrade (UGradeID, Name) Values (254, '������')
INSERT INTO UserGrade (UGradeID, Name) Values (255, '������')

/* �����̿��� ���� ���(PremiumGrade) */
INSERT INTO PremiumGrade (PGradeID, Name) Values (0, '����')

/* ����Ÿ�� ���(GameType) */
INSERT INTO GameType (GameTypeID, Name) Values (0, 'Solo Death Match')
INSERT INTO GameType (GameTypeID, Name) Values (1, 'Team Death Match')
INSERT INTO GameType (GameTypeID, Name) Values (2, 'Solo Gladiator')
INSERT INTO GameType (GameTypeID, Name) Values (3, 'Team Gladiator')
INSERT INTO GameType (GameTypeID, Name) Values (4, 'Assassinate')

/* ȭ�� ����(Currency) */
INSERT INTO Currency (CurrencyID, Name) Values (0, '��')

/* ���� ���� ���(BillingMethod) */
INSERT INTO BillingMethod (BillingMethodID, Name) Values (0, '������ü')
INSERT INTO BillingMethod (BillingMethodID, Name) Values (1, '�ſ�ī��')
INSERT INTO BillingMethod (BillingMethodID, Name) Values (2, '�ڵ���')
INSERT INTO BillingMethod (BillingMethodID, Name) Values (3, '700����')
INSERT INTO BillingMethod (BillingMethodID, Name) Values (4, 'ADSL')

/* ������ ���� ���(PurchaseMethod) */
INSERT Into PurchaseMethod (PurchaseMethodID, Name) Values (0, '����')
INSERT Into PurchaseMethod (PurchaseMethodID, Name) Values (1, '�ٿ�Ƽ')
INSERT Into PurchaseMethod (PurchaseMethodID, Name) Values (2, '����&�ٿ�Ƽ')
INSERT Into PurchaseMethod (PurchaseMethodID, Name) Values (3, 'Ư��(�̺�Ʈ)')

/* �� */
INSERT Into Map (MapID, Name, MaxPlayer) values (0, 'Mansion', 32)
INSERT Into Map (MapID, Name, MaxPlayer) values (1, 'Prison', 32)
INSERT Into Map (MapID, Name, MaxPlayer) values (2, 'Prison II', 32)
INSERT INTO Map (MapID, Name, MaxPlayer) values (3, 'Station', 32)

/* �������� */
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (1, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (2, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (3, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (4, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (5, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (6, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (7, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (8, 0, 0)
INSERT Into ServerStatus (ServerID, CurrPlayer, MaxPlayer) Values (9, 0, 0)


go
