extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %MakeCoffee:
		person.do_walk_to(global_position)
		person.do_start_use(self)
		person.do_wait(1.0, 2.0)
		person.do_gain("res://objects/Coffee.tscn")
		person.do_stop_use(self)
		person.do_loiter(2.5) # it's too hot
