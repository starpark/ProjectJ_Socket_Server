DROP TABLE IF EXISTS [dbo].[MatchPlayers]
CREATE TABLE [dbo].[MatchPlayers]
(
	[match_player_id] INT NOT NULL PRIMARY KEY IDENTITY,
	[match_id] int FOREIGN KEY REFERENCES [dbo].[MatchRecords](match_id),
	[player_id] int FOREIGN KEY REFERENCES [dbo].[PlayerAccount](player_id),
	[is_chaser] BIT,
	[is_fugitive] BIT,
	[kills] TINYINT DEFAULT 0,
	[is_death] BIT,
	[is_escape] BIT,
	[score] INT NOT NULL DEFAULT 0
)