extends StaticBody2D

# Provides coffee for people.

func plan(person: Person, action: Action) -> void:
	if action == %MakeCoffee:
		# The person is interested in getting coffee
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(1.0, 2.0)
		person.do_gain("res://objects/Coffee.tscn")
		person.do_stop_use(self)
		
		# It's hot, so linger briefly
		person.do_loiter(2.5)
		
		# No reward, as the person has to drink the coffee to get that
