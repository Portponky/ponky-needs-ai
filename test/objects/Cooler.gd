extends Node2D


func plan(person: Person, action: Action) -> void:
	if action == %Drink:
		person.do_walk_to(global_position)
		person.do_wait(3.0, 4.0)
		person.do_reward(action.advert)
