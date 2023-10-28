#include "pch.h"
#include "../GameServer/Player.h"
#include "../GameServer/Room.h"
#include "../GameServer/Inventory.h"

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
