#pragma once
#include "Game.hpp"

// you control a player and a group of characters
// there are two modes turn-based "battle" and "preparation"
// in preparation you choose charácters, abilities, weapons and stances
// and in battle you use those to fight against enemies
// you collect items in battles
// characters have special abilities and combos
// abilities can combo together
// belts limit the amount of items you can carry into battle using slots
// you start with very limited equipment
// armor can have (dimension) slots
// backpacks can be taken into battle too
// tiled (checkerboard) fight area
// switching weapons or abilities during battle uses turns
// stances determine special buffs and debuffs for the weapon you are using
// multiple enemy types

// FOLLOWERS
// merchant

// ENEMIES
// bandit
// slime
// warthog
// knight

// ITEMS
// sword
// ring
// belt
// backpack
// armor
// shoes
// firestone (enchants weapon)

struct KevaCharacter
{
	// slots for fight
	Index headSlot        = 0;
	Index armorSlot       = 0;
	Index backSlot        = 0;
	Index glovesSlot      = 0;
	Index leftWeaponSlot  = 0;
	Index rightWeaponSlot = 0;
	Index beltSlot        = 0;
	Index pantsSlot       = 0;
	Index shoesSlot       = 0;
};

struct KevaPlayer
{
	Vector<Index> inventory; // infinite item storage
	KevaCharacter mainCharacter;
};

enum class KevaSlot
{
	Item      = 0,
	Head      = 1,
	Armor     = 2,
	Back      = 4,
	Gloves    = 8,
	OneHanded = 16,
	TwoHanded = 32,
	Belt      = 64,
	Pants     = 128,
	Shoes     = 256
};

struct KevaItem
{
	Vector<Index> storedItems;
	ULL           itemFlags = 0;
	Float         weight    = 0.0;
	Float         volume    = 0.0;
	String        name      = "Error";

	explicit KevaItem(const String& name)
	{
		this->name = name;
	}

	explicit KevaItem(const String& name, const ULL itemFlags)
	{
		this->name      = name;
		this->itemFlags = itemFlags;
	}
};

struct KevaSimulation : GameSimulation
{
	Bool keeprunning = true;

	Bool                  inBattle = false;
	Vector<KevaItem>      items;
	Vector<KevaCharacter> people;
	KevaPlayer            player;

	void create() override;
	void destroy() override;
	void printOptions() const;
	void update(Float delta) override;
};

struct Keva
{
	KevaSimulation sim;

	void run();
};
