DROP TABLE IF EXISTS [dbo].[PlayerStatistics]
CREATE TABLE [dbo].[PlayerStatistics]
(
	[player_id] INT NOT NULL PRIMARY KEY IDENTITY FOREIGN KEY REFERENCES [dbo].[PlayerAccounts](player_id),
	[kill_count] INT NOT NULL DEFAULT 0,
	[escape_count] INT NOT NULL DEFAULT 0,
	[total_score] BIGINT NOT NULL DEFAULT 0
)