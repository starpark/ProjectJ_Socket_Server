DROP TABLE IF EXISTS [dbo].[PlayerAccount]
CREATE TABLE [dbo].[PlayerAccount]
(
	[player_id] INT NOT NULL PRIMARY KEY IDENTITY,
	[name] NVARCHAR(50) NOT NULL,
	[nickname] NVARCHAR(50) NOT NULL,
	[password] NVARCHAR(50) NOT NULL,
	[registration_date] DATETIME NOT NULL DEFAULT GETDATE()
)