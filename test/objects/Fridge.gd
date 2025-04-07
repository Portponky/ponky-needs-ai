extends StaticBody2D

# A refridgerator that allows people to get food.
# What they get is random.

func plan(person: Person, action: Action) -> void:
	if action == %GrabSomething:
		# This person wants food from the fridge
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(0.6, 1.5)
		
		# What they get is random
		var r = randf()
		if r < 0.08:
			# Lucky dip! A meal that's ready to eat!
			person.do_gain("res://objects/CookedMeal.tscn")
		elif r < 0.65:
			# Most likely - just a regular sandwich
			person.do_gain("res://objects/Sandwich.tscn")
		else:
			# A frozen meal
			person.do_gain("res://objects/FrozenMeal.tscn")
		
		person.do_stop_use(self)
		person.do_loiter(0.5)
		
		# No reward, but the food they are holding can be eaten for hunger value
