extends Node2D

# A vending machine that can provide snacks. Can give some basic hunger needs.
# It is very flashy so people will only really use it if they are nearby.

func plan(person: Person, action: Action) -> void:
	if action == %GetSnacks:
		# Go grab some snacks
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(2.0, 3.0)
		person.do_gain("res://objects/Snack.tscn")
		person.do_stop_use(self)
		
		# Regret decision
		person.do_loiter(1.5)
		
		# No reward, the person has to eat the snack to feel less hungry
