#include "pch.h"
#include "../GameServer/Player.h"
#include "../GameServer/Room.h"
#include "../GameServer/Lobby.h"
#include "../GameServer/Inventory.h"
#include "../GameServer/Message.pb.h"
#include "../GameServer/GameSession.h"
#include <random>

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(InventoryTest, HandleItem)
{
	auto player = std::make_shared<Player>();
	Inventory iv;

	ProjectJ::Vector vector1;
	ProjectJ::Rotator rotator1;
	Point point = {2, 3};
	auto a = make_shared<Item>(1, 1000, point, vector1, rotator1);
	auto b = make_shared<Item>(2, 1000, point, vector1, rotator1);
	auto c = make_shared<Item>(3, 1000, point, vector1, rotator1);

	EXPECT_EQ(iv.TryAddItem(a, player), true);
	EXPECT_EQ(iv.TryAddItem(b, player), true);
	EXPECT_EQ(iv.TryAddItem(c, player), true);
	EXPECT_EQ(iv.TryAddItem(a, player), false);

	EXPECT_EQ(iv.DropItem(a, vector1, rotator1), true);
	EXPECT_EQ(iv.DropItem(a, vector1, rotator1), false);

	auto d = make_shared<Item>(5, 155000, point, vector1, rotator1);
	EXPECT_EQ(iv.TryAddItem(d, player), false);

	Point bigPoint = {30, 30};
	auto e = make_shared<Item>(5, 1, bigPoint, vector1, rotator1);
	EXPECT_EQ(iv.TryAddItem(e, player), false);

	iv.PrintTest();
	EXPECT_TRUE(iv.RelocateItem(b, 60, true));
	EXPECT_FALSE(iv.RelocateItem(b, -1, true));
	iv.PrintTest();

	EXPECT_FALSE(iv.RelocateItem(e, 0, true));
}

TEST(InventoryTest, FullItems)
{
	auto player = std::make_shared<Player>();
	Inventory iv;

	ProjectJ::Vector vector1;
	ProjectJ::Rotator rotator1;
	Point point = {1, 1};

	vector<shared_ptr<Item>> items;

	for (int i = 1; i <= 90; i++)
	{
		auto newItem = make_shared<Item>(i, 1, point, vector1, rotator1);
		items.push_back(newItem);
		EXPECT_EQ(iv.TryAddItem(newItem, player), true);
	}
	iv.PrintTest();

	for (int i = 0; i < 90; i++)
	{
		EXPECT_TRUE(iv.RelocateItem(items[i], i, false));
	}

	for (int i = 0; i < 90; i++)
	{
		shared_ptr<Item> item = items.back();
		items.pop_back();
		EXPECT_TRUE(iv.DropItem(item, vector1, rotator1));
	}
	iv.PrintTest();

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(2, 4);

	for (int i = 0; i < 5; i++)
	{
		Point randPoint = {dis(gen), dis(gen)};
		auto newItem = make_shared<Item>(i, 1, randPoint, vector1, rotator1);
		items.push_back(newItem);
		EXPECT_EQ(iv.TryAddItem(newItem, player), true);
	}
	iv.PrintTest();
}

TEST(ProtobufTest, RepeatedTest)
{
	ProjectJ::S_LOBBY_REFRESH_ROOM packet;
	{
		auto room = packet.add_rooms();
		room->set_number_of_player(1);
		room->set_title("test_room1");
		room->set_id(1);
	}
	{
		auto room = packet.add_rooms();
		room->set_number_of_player(2);
		room->set_title("test_room2");
		room->set_id(2);
	}
	{
		auto room = packet.add_rooms();
		room->set_number_of_player(3);
		room->set_title("test_room3");
		room->set_id(3);
	}

	for (auto room : packet.rooms())
	{
		cout << room.id() << ": " << room.title() << " " << endl;
	}

	for (int i = 0; i < packet.rooms_size(); i++)
	{
		ProjectJ::Room room = packet.rooms(i);
		cout << room.id() << ": " << room.title() << " " << endl;
	}
}

TEST(LobbyRoomMatchTest, HandleLobby)
{
	auto dummySession = make_shared<GameSession>();
	auto dummyLobby = make_shared<Lobby>();
	dummySession->SetID(1);
	dummySession->SetNickname("dummy");
	dummySession->SetName("dummy");
}

/*TEST(LobbyTest, HandleRoomCreateAndLeave)
{
	auto player = std::make_shared<GameSession>();
	Lobby lb;
	EXPECT_TRUE(lb.CreateRoom(player, "TestRoom"));
	auto size = lb.GetRoomList().size();
	EXPECT_EQ(size, 1);
	lb.LeaveRoom(1, player);
	size = lb.GetRoomList().size();
	EXPECT_EQ(size, 0);
}*/
