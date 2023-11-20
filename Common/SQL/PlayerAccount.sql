DROP TABLE IF EXISTS [dbo].[PlayerAccounts]
CREATE TABLE [dbo].[PlayerAccounts]
(
	[player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),
	[name] NVARCHAR(20) NOT NULL,
	[nickname] NVARCHAR(12) NOT NULL,
	[password] BINARY(60) NOT NULL,
	[registration_date] DATETIME2 NOT NULL  DEFAULT SYSDATETIME()
)