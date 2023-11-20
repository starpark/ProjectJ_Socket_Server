DROP TABLE IF EXISTS [dbo].[MatchPlayers]
CREATE TABLE [dbo].[MatchPlayers]
(
	[match_player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),
	[match_id] UNIQUEIDENTIFIER NOT NULL,
	[player_id] INT NOT NULL,
	[is_chaser] BIT NOT NULL,
	[is_fugitive] BIT NOT NULL,
	[kills] TINYINT NOT NULL,
	[is_murdered] BIT NOT NULL,
	[escaped] BIT NOT NULL,
	[score] INT NOT NULL,
	FOREIGN KEY(match_id) REFERENCES [dbo].[MatchRecords](match_id),
	FOREIGN KEY(player_id) REFERENCES [dbo].[PlayerAccounts](player_id)
)