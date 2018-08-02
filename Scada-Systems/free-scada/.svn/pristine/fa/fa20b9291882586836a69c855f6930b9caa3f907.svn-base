CREATE TABLE Servers( 
	ID AUTOINCREMENT,
	ServerGUID VARCHAR(40) NOT NULL,
	Computer VARCHAR(128) NOT NULL,
	Description TEXT,
	PRIMARY KEY(ID),
	UNIQUE (ServerGUID, Computer));

CREATE TABLE Channels(
	ID AUTOINCREMENT,
	ServerID INTEGER NOT NULL,
	Name VARCHAR(255) NOT NULL,
	Description TEXT,
	PRIMARY KEY(ID),
	UNIQUE ( Name, ServerID ),
	FOREIGN KEY (ServerID) REFERENCES Servers(ID));
	
CREATE TABLE ChannelAttributes(
	ChannelID INTEGER NOT NULL,
	Attribute VARCHAR(128) NOT NULL,
	[Value] VARCHAR(64) NOT NULL,
	PRIMARY KEY(ChannelID, Attribute),
	UNIQUE ( ChannelID, Attribute ),
	FOREIGN KEY (ChannelID) REFERENCES Channels(ID));
	
CREATE TABLE EventTypes(
	ID INTEGER NOT NULL,
	Name VARCHAR(255) NOT NULL,
	Description TEXT,
	PRIMARY KEY(ID));
	
CREATE TABLE Events(
	ID AUTOINCREMENT,
	ChannelID INTEGER NOT NULL,
	TypeID INTEGER NOT NULL,
	EventTime DATETIME NOT NULL,
	PRIMARY KEY(ID),
	FOREIGN KEY (ChannelID) REFERENCES Channels(ID),
	FOREIGN KEY (TypeID) REFERENCES EventTypes(ID));
	
CREATE TABLE BooleanChannelData(
	ID AUTOINCREMENT,
	ChannelID INTEGER NOT NULL,
	EventTime DATETIME NOT NULL,
	[Value] BIT NOT NULL,
	Quality INTEGER,
	PRIMARY KEY(ID),
	FOREIGN KEY (ChannelID) REFERENCES Channels(ID));
	
CREATE TABLE DWordChannelData(
	ID AUTOINCREMENT,
	ChannelID INTEGER NOT NULL,
	EventTime DATETIME NOT NULL,
	[Value] INTEGER NOT NULL,
	Quality INTEGER,
	PRIMARY KEY(ID),
	FOREIGN KEY (ChannelID) REFERENCES Channels(ID));
	
CREATE TABLE FloatChannelData(
	ID AUTOINCREMENT,
	ChannelID INTEGER NOT NULL,
	EventTime DATETIME NOT NULL,
	[Value] FLOAT NOT NULL,
	Quality INTEGER,
	PRIMARY KEY(ID),
	FOREIGN KEY (ChannelID) REFERENCES Channels(ID));
	
CREATE TABLE Users(
	ID AUTOINCREMENT,
	[Login] VARCHAR(64) NOT NULL,
	[Password] VARCHAR(32),
	[FirstName] VARCHAR(128),
	[MiddleName] VARCHAR(128),
	[LastName] VARCHAR(128),
	PRIMARY KEY(Login),
	UNIQUE (ID));
	
CREATE TABLE UserPermissions(
	UserID INTEGER NOT NULL,
	Permission VARCHAR(128) NOT NULL,
	[Value] VARCHAR(64) NOT NULL,
	PRIMARY KEY(UserID, Permission),
	UNIQUE ( UserID, Permission ),
	FOREIGN KEY (UserID) REFERENCES Users(ID));
	
INSERT INTO Users (Login,[Password]) VALUES('Administrator','');

INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanEditUsers', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));

INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanViewUsers', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));
	
INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanViewData', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));
	
INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanEditData', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));
	
INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanEditChannels', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));
	
INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanEditEvents', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));

INSERT INTO UserPermissions ( UserID, Permission, [Value] )
	SELECT Users.ID, 'CanViewEvents', 'yes'
	FROM Users
	WHERE (((Users.Login)='Administrator'));
	
	
INSERT INTO EventTypes (ID, Name) VALUES (0, 'Channel changing');
INSERT INTO EventTypes (ID, Name) VALUES (1, 'Channel value is increased');
INSERT INTO EventTypes (ID, Name) VALUES (2, 'Channel value is decreased');
INSERT INTO EventTypes (ID, Name) VALUES (3, 'Value crossed maximum constant');
INSERT INTO EventTypes (ID, Name) VALUES (4, 'Channel goes to normal (max)');
INSERT INTO EventTypes (ID, Name) VALUES (5, 'Value crossed minimum constant');
INSERT INTO EventTypes (ID, Name) VALUES (6, 'Channel goes to normal (min)');
