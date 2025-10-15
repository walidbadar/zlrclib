#include <app/zlrclib.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_tracks);

void zlrclib_track_info(struct zlrclib_track *track)
{
	if (!track) {
		LOG_ERR("Track info is NULL.");
		return;
	}

	LOG_INF("ID: %u", track->id);
	LOG_INF("Name: %s", track->name);
	LOG_INF("Track Name: %s", track->track_name);
	LOG_INF("Artist Name: %s", track->artist_name);
	LOG_INF("Album Name: %s", track->album_name);
	LOG_INF("Instrumental: %s", track->instrumental ? "Yes" : "No");
	LOG_INF("Plain Lyrics: %s", track->plain_lyrics);
	LOG_INF("Synced Lyrics: %s", track->synced_lyrics);
}

uint8_t track1[] = "{"
		   "\"id\": 479074,"
		   "\"name\": \"You Suffer\","
		   "\"trackName\": \"You Suffer\","
		   "\"artistName\": \"Napalm Death\","
		   "\"albumName\": \"Scum\","
		   "\"duration\": 5,"
		   "\"instrumental\": false,"
		   "\"plainLyrics\": \"You suffer, but why?\","
		   "\"syncedLyrics\": \"[00:00.57] You suffer, but why?\n[00:01.83] \""
		   "}";

uint8_t track2[] =
	"{"
	"\"id\": 19882463,"
	"\"name\": \"Mystical Magical\","
	"\"trackName\": \"Mystical Magical\","
	"\"artistName\": \"Benson Boone\","
	"\"albumName\": \"2:46\","
	"\"duration\": 165.72,"
	"\"instrumental\": false,"
	"\"plainLyrics\": \"There's nothing colder\nThan your shoulder\nWhen you're "
	"dragging me along\nLike you do, like you do\n\nAnd then you switch up\nWith no "
	"warning\nAnd you kiss me like you want it\nHow rude, how rude\n\nBut I kinda like "
	"it anyways\nI don't mind\nIf this is gonna take a million days\nI know you'll "
	"come around\nTo me eventually\nIf you sit back, relax\nEnjoy my company\nMy "
	"company\n\nOnce you know\nWhat my love's gonna feel like\nNothing else\nWill feel "
	"right\nYou can feel like\nMoonbeam ice cream\nTaking off your blue jeans\nDancing "
	"at the movies\n\n'Cause it feels\nSo mystical\nMagical oh baby\n'Cause once you "
	"know\nOnce you know\nMy love is\nSo mystical, magical\nOh baby!\n'Cause once you "
	"know\nOnce you know\n\nMy little hard-to-get baby\nI want to give you the "
	"world\nNot saying you gotta chase me\nBut I wouldn't mind it\nIf you gave me just "
	"a little bit of\nSomething we can work it with\nBut all you do is push me "
	"out\n\nBut I like it anyways\n'Cause I think\nI'm getting closer to you "
	"everyday\nI know you'll come around\nTo me eventually\nJust sit back, "
	"relax\nEnjoy my company\n\nOh baby! Trust me\nOnce you know\nWhat my love's gonna "
	"feel like\nNothing else will feel right\nYou can feel like\nMoonbeam ice "
	"cream\nTaking off your blue jeans\nDancing at the movies\n\n'Cause it feels\nSo "
	"mystical, magical\nOh baby!\n'Cause, once you know\nOnce you know\nMy love is\nSo "
	"mystical, magical\nOh baby!\n'Cause once you know\nOnce you know\n\nOnce you know "
	"what my love's going to feel like\nMoonbeam ice cream\nNothing else will feel "
	"right\n\nMystical, magical oh baby\nIt's mystical, magical\","
	"\"syncedLyrics\": \"[00:07.63] There's nothing colder\n[00:08.96] Than your "
	"shoulder\n[00:09.75] When you're dragging me along\n[00:11.69] Like you do, like "
	"you do\n[00:15.42] And then you switch up\n[00:16.56] With no warning\n[00:17.77] "
	"And you kiss me like you want it\n[00:19.55] How rude, how rude\n[00:22.76] But I "
	"kinda like it anyways\n[00:25.70] I don't mind\n[00:26.57] If this is gonna take "
	"a million days\n[00:29.54] I know you'll come around\n[00:31.41] To me "
	"eventually\n[00:33.57] If you sit back, relax\n[00:35.66] Enjoy my "
	"company\n[00:37.83] My company\n[00:39.81] Once you know\n[00:41.65] What my "
	"love's gonna feel like\n[00:44.11] Nothing else\n[00:45.07] Will feel "
	"right\n[00:46.31] You can feel like\n[00:47.96] Moonbeam ice cream\n[00:50.08] "
	"Taking off your blue jeans\n[00:52.19] Dancing at the movies\n[00:54.38] 'Cause "
	"it feels\n[00:55.41] So mystical\n[00:57.34] Magical oh baby\n[00:59.52] 'Cause "
	"once you know\n[01:01.11] Once you know\n[01:02.58] My love is\n[01:03.61] So "
	"mystical, magical\n[01:06.46] Oh baby!\n[01:07.76] 'Cause once you "
	"know\n[01:09.15] Once you know\n[01:12.36] My little hard-to-get baby\n[01:14.29] "
	"I want to give you the world\n[01:16.43] Not saying you gotta chase "
	"me\n[01:18.23] But I wouldn't mind it\n[01:20.04] If you gave me just a little "
	"bit of\n[01:22.09] Something we can work it with\n[01:24.04] But all you do is "
	"push me out\n[01:27.61] But I like it anyways\n[01:29.63] 'Cause I "
	"think\n[01:30.54] I'm getting closer to you everyday\n[01:33.79] I know you'll "
	"come around\n[01:35.38] To me eventually\n[01:37.76] Just sit back, "
	"relax\n[01:39.60] Enjoy my company\n[01:42.14] Oh baby! Trust me\n[01:44.18] Once "
	"you know\n[01:45.66] What my love's gonna feel like\n[01:48.15] Nothing else will "
	"feel right\n[01:50.44] You can feel like\n[01:52.06] Moonbeam ice "
	"cream\n[01:53.94] Taking off your blue jeans\n[01:56.11] Dancing at the "
	"movies\n[01:58.37] 'Cause it feels\n[01:59.28] So mystical, magical\n[02:02.52] "
	"Oh baby!\n[02:03.68] 'Cause, once you know\n[02:05.26] Once you know\n[02:06.51] "
	"My love is\n[02:07.54] So mystical, magical\n[02:10.31] Oh baby!\n[02:11.68] "
	"'Cause once you know\n[02:13.60] Once you know\n[02:16.69] Once you know what my "
	"love's going to feel like\n[02:21.50] \n[02:24.49] Moonbeam ice cream\n[02:26.12] "
	"Nothing else will feel right\n[02:29.71] \n[02:32.04] Mystical, magical oh "
	"baby\n[02:35.89] It's mystical, magical\n[02:38.94] \""
	"}";
