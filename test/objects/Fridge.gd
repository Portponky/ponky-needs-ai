extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %GrabSomething:
		person.do_walk_to(global_position)
		person.do_start_use(self)
		person.do_wait(0.6, 1.5)
		person.do_gain("res://objects/Sandwich.tscn")
		person.do_stop_use(self)
		person.do_loiter(0.5)
