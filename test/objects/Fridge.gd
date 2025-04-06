extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %GrabSomething:
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(0.6, 1.5)
		var r = randf()
		if r < 0.08:
			# lucky dip! a meal that's ready to eat!
			person.do_gain("res://objects/CookedMeal.tscn")
		elif r < 0.65:
			# most likely - just a regular sandwich
			person.do_gain("res://objects/Sandwich.tscn")
		else:
			# a frozen meal
			person.do_gain("res://objects/FrozenMeal.tscn")
		person.do_stop_use(self)
		person.do_loiter(0.5)
