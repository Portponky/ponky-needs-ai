extends Action

func plan(person: Person) -> void:
	person.do_walk_to(global_position)
	person.do_wait(3.0, 4.0)
	person.do_reward(advert)
