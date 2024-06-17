#pragma once

#include "Basics.hpp"

//ankus
//arbalest
//assegai
//atlatl
//awl pike
//axe
//baton
//balisong
//ballista
//bardiche
//billhook
//blade
//blowgun
//bludgeon
//brandistock
//brass knuckles
//broadsword
//bo staff
//bolas
//boomerang
//bow
//boxing gloves
//bunka bocho
//caltrop
//cane sword
//cannon
//cat o' nine tails
//cestus
//chain
//chain Whip
//chakram
//clava
//claws
//claymore
//club
//crossbow
//crowbar
//cudgel
//cutlass
//dagger
//dao
//dart
//estoc
//falcata
//falchion
//flail
//flamberge
//gandasa
//garrote
//gladius
//glaive
//grenade
//guisarme
//halberd
//hammer
//harpoon
//hatchet
//hook
//javelin
//kama
//katana
//katar
//khopesh
//knife
//kubotan 
//kunai
//kusarigama
//lance
//lasso
//lathi
//longbow
//mace
//machete
//macuahuitl
//maul
//morning star
//musket
//naginata
//net
//nodachi
//nunchaku
//parang
//partisan
//pellet bow
//pick
//pickaxe
//pilum
//pike
//pistol
//polearm
//push dagger
//quarterstaff
//revolver
//rapier
//rifle
//rungu
//sabre
//sai
//scepter
//scimitar
//scythe
//shuriken
//shamshir
//shield
//sickle
//sling
//sling staff
//spear
//staff
//stiletto
//sword
//tetsubo
//trident
//tomahawk
//tonfa
//whip


struct DNDBAse
{
	Vector<String> weapons;
	Vector<String> weaponTypes;

	DNDBAse(){



		weaponTypes.emplace_back("Ranged");
		weaponTypes.emplace_back("Meelee");
		weaponTypes.emplace_back("Trap");

	}
};