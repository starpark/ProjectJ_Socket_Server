DROP TABLE IF EXISTS [dbo].[PlayerAccount]
CREATE TABLE [dbo].[PlayerAccount]
(
	[player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),
	[name] NVARCHAR(10) NOT NULL,
	[nickname] NVARCHAR(10) NOT NULL,
	[password] NVARCHAR(20) NOT NULL,
	[registration_date] DATETIME NOT NULL DEFAULT GETDATE()
)