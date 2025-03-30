extends StaticBody2D

func plan(person: Person, action: Action) -> void:
	if action == %FilePaper:
		person.do_walk_to(global_position)
		person.do_wait(1.0, 2.0)
		person.do_destroy()
		person.do_reward(action.advert)
