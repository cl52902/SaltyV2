#pragma once

namespace big
{
	class gui
	{
	public:
		void dx_init();
		void dx_on_tick();

		void script_init();
		void script_on_tick();
		static void script_func();
	public:
		bool m_opened{};
	};

	class config
	{
	public:
		bool protection = false;
	};

	inline config g_config;
	inline gui g_gui;
}
