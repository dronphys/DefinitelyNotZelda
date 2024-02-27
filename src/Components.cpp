#include "Components.h"


PlayerState operator|(PlayerState a, PlayerState b) {
	return static_cast<PlayerState>(static_cast<int>(a) |
		static_cast<int>(b));
}

PlayerState operator&(PlayerState a, PlayerState b) {
	return static_cast<PlayerState>(static_cast<int>(a) &
		static_cast<int>(b));
}