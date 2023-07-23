package edu.skku.cs.pp

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.CalendarView
import android.widget.EditText
import android.widget.TextView
import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.MediaType
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException
import java.time.LocalDate

class AddCalendar : AppCompatActivity() {

    data class Calendar(
        val name: String,
        val events: List<Event>
    )

    data class Event(
        val date: String,
        val title: String,
        val description: String,
        val check: Boolean
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_add_calendar)

        supportActionBar?.setTitle("DayFlow")

        val newCalendarTitle = findViewById<EditText>(R.id.newCalendarTitle)
        val eventTitleInput = findViewById<EditText>(R.id.eventTitleInput)
        val eventDescInput = findViewById<EditText>(R.id.eventDescInput)
        val tempCalendar = findViewById<CalendarView>(R.id.calendarView)
        val currentDate = findViewById<TextView>(R.id.currentDate2)
        var selectedDate = LocalDate.now().toString()
        currentDate.text = selectedDate

        tempCalendar.setOnDateChangeListener { calendarView, year, month, dayOfMonth ->
            selectedDate = String.format("%04d-%02d-%02d", year, month + 1, dayOfMonth)
            currentDate.text = selectedDate
        }

        val submitBtn = findViewById<Button>(R.id.newCalendarSubmit)

        val client = OkHttpClient()

        submitBtn.setOnClickListener {
            val calendarTitle = newCalendarTitle.text.toString()
            Log.i("test", calendarTitle)
            val eventTitle = eventTitleInput.text.toString()
            val eventDescription = eventDescInput.text.toString()
            val eventDate = selectedDate

            val jsonEventData = """
            {
                "calendar": "$calendarTitle",
                "date": "$eventDate",
                "title": "$eventTitle",
                "description": "$eventDescription",
                "check": false
            }
            """.trimIndent()

            val url = "url_link"
            val requestBody = jsonEventData.toRequestBody("application/json".toMediaTypeOrNull())
            val request = Request.Builder().url(url).post(requestBody).build()

            client.newCall(request).enqueue(object : okhttp3.Callback {
                override fun onFailure(call: okhttp3.Call, e: IOException) {
                    e.printStackTrace()
                }

                override fun onResponse(call: okhttp3.Call, response: okhttp3.Response) {
                    if (!response.isSuccessful) throw IOException("Unexpected code $response")
                    CoroutineScope(Dispatchers.Main).launch{
                        finish()
                    }
                }
            })
        }
    }
}