DROP TABLE IF EXISTS [dbo].[MatchPlayers]
CREATE TABLE [dbo].[MatchPlayers]
(
	[match_player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),
	[match_id] int NOT NULL FOREIGN KEY REFERENCES [dbo].[MatchRecords](match_id),
	[player_id] int NOT NULL FOREIGN KEY REFERENCES [dbo].[PlayerAccount](player_id),
	[is_chaser] BIT NOT NULL,
	[is_fugitive] BIT NOT NULL,
	[kills] TINYINT NOT NULL,
	[is_death] BIT NOT NULL,
	[is_escape] BIT NOT NULL,
	[score] INT NOT NULL
)