DROP TABLE IF EXISTS [dbo].[MatchRecords]
CREATE TABLE [dbo].[MatchRecords]
(
	[match_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),
	[start_time] DATETIME NOT NULL DEFAULT GETDATE(),
	[end_time] DATETIME
)