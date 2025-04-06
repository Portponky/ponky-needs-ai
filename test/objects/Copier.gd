extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %CopySomething:
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_destroy()
		person.do_wait(6.0, 8.0)
		person.do_gain("res://objects/Paper.tscn")
		person.do_drop()
		person.do_gain("res://objects/Paper.tscn")
		person.do_stop_use(self)
		person.do_reward(action.advert)
		%CopySomething.active = false
		%Timer.start()


func _on_timer_timeout() -> void:
	%CopySomething.active = true
