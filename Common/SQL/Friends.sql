DROP TABLE IF EXISTS [dbo].[Friends]
CREATE TABLE [dbo].[Friends]
(
	[friend_one] INT NOT NULL,
	[friend_two] INT NOT NULL,
	[create_date] DATETIME NOT NULL,
	[friendship_date] DATETIME,
	[status] NVARCHAR(20) NOT NULL DEFAULT 'WAITING',
	FOREIGN KEY([friend_one]) REFERENCES [dbo].[PlayerAccounts](player_id),
	FOREIGN KEY([friend_two]) REFERENCES [dbo].[PlayerAccounts](player_id),
	PRIMARY KEY([friend_one], [friend_two])
)