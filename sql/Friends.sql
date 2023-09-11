DROP TABLE IF EXISTS [dbo].[Friends]
CREATE TABLE [dbo].[Friens]
(
	[friend_one] INT NOT NULL,
	[friend_two] INT NOT NULL,
	[create_date] DATETIME NOT NULL,
	[update_date] DATETIME NULL,
	[status] NVARCHAR(50) NOT NULL,
	FOREIGN KEY([friend_one]) REFERENCES [dbo].[PlayerAccount](player_id),
	FOREIGN KEY([friend_two]) REFERENCES [dbo].[PlayerAccount](player_id),
	PRIMARY KEY([friend_one], [friend_two])
)