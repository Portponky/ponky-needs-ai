extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	person.do_walk_to(self)
	person.do_start_use(self)
	person.do_destroy()
	person.do_wait(3.0, 4.0)
	if action == %HeatCoffee:
		person.do_gain("res://objects/Coffee.tscn")
		person.do_reward({"social": 0.05, "clean": 0.05}) # give partial reward
	elif action == %HeatMeal:
		person.do_gain("res://objects/CookedMeal.tscn")
		person.do_reward({"clean": 0.05}) # give partial reward
	person.do_stop_use(self)
	person.do_loiter(1.5) # it's hot!
	
