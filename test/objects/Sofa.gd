extends Node2D

func plan(person: Person, action: Action) -> void:
	if action == %Relax:
		person.do_walk_to(global_position)
		person.do_loiter(2.0)
		person.do_walk_to(global_position)
		person.do_loiter(2.0)
		person.do_reward(action.advert)
	elif action == %Sit:
		person.do_walk_to(global_position)
		person.do_start_use(self)
		person.do_wait(6.0, 8.0)
		person.do_stop_use(self)
		person.do_reward(action.advert)
