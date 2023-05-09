#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <vector>
#include <map>

class Handler
{
	public:
		Handler();
		~Handler();

		void	parseRequest();
		void	parseBody();

	private:
		void	handlePost();
		void	handleGet();
		void	handleDelete();
};

#endif
