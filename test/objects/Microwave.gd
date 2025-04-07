extends StaticBody2D

# A microwave that heats things.
# At the moment that's just coffee and frozen meals.

func plan(person: Person, action: Action) -> void:
	# Regardless of action chosen, go to the microwave
	person.do_walk_to(self)
	person.do_start_use(self)
	
	# 'Put' the item into the microwave (destroy item) and wait for it to be heated
	person.do_destroy()
	person.do_wait(3.0, 4.0)
	
	if action == %HeatCoffee:
		# We heated coffee, so just generate a new cup
		person.do_gain("res://objects/Coffee.tscn")
		person.do_reward({"social": 0.05, "clean": 0.05}) # give partial reward
	elif action == %HeatMeal:
		# We heated a frozen meal, so generate a cooked meal
		person.do_gain("res://objects/CookedMeal.tscn")
		person.do_reward({"clean": 0.05}) # give partial reward
	
	person.do_stop_use(self)
	
	# Item is hot so loiter for a bit. The person can get the reward by consuming
	# whatever they were trying to heat.
	person.do_loiter(1.5)
