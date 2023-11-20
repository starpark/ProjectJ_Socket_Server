DROP TABLE IF EXISTS [dbo].[MatchRecords]
CREATE TABLE [dbo].[MatchRecords]
(
	[match_id] UNIQUEIDENTIFIER DEFAULT NEWID(),
	[created_at] DATETIME2 DEFAULT SYSDATETIME(),
	[start_time] DATETIME2, 
	[end_time] DATETIME2,

	PRIMARY KEY(match_id)
)