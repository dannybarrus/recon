using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace Recon.WebApi
{
    [ApiController]
    [Route("api/[controller]")]
    [Authorize]
    public class UsersController : ControllerBase
    {
        private readonly IUserService _service;
        private readonly ILogger<UsersController> _logger;

        public UsersController(IUserService service, ILogger<UsersController> logger)
            => (_service, _logger) = (service, logger);

        [HttpGet("{id:int}")]
        [ProducesResponseType(typeof(UserDto), StatusCodes.Status200OK)]
        [ProducesResponseType(StatusCodes.Status404NotFound)]
        public async Task<IActionResult> GetAsync(int id, CancellationToken ct)
        {
            var user = await _service.GetByIdAsync(id, ct);
            return user is null ? NotFound() : Ok(user);
        }

        [HttpGet]
        [ProducesResponseType(typeof(IEnumerable<UserDto>), StatusCodes.Status200OK)]
        public async Task<IActionResult> GetAllAsync(CancellationToken ct)
        {
            var users = await _service.GetAllAsync(ct);
            return Ok(users);
        }

        [HttpPost]
        [ProducesResponseType(typeof(UserDto), StatusCodes.Status201Created)]
        [ProducesResponseType(StatusCodes.Status400BadRequest)]
        public async Task<IActionResult> CreateAsync(CreateUserRequest request, CancellationToken ct)
        {
            if (!ModelState.IsValid) return BadRequest(ModelState);

            var user = await _service.CreateAsync(request, ct);
            return CreatedAtAction(nameof(GetAsync), new { id = user.Id }, user);
        }

        [HttpPut("{id:int}")]
        [ProducesResponseType(StatusCodes.Status204NoContent)]
        [ProducesResponseType(StatusCodes.Status404NotFound)]
        public async Task<IActionResult> UpdateAsync(int id, UpdateUserRequest request, CancellationToken ct)
        {
            var updated = await _service.UpdateAsync(id, request, ct);
            return updated ? NoContent() : NotFound();
        }

        [HttpDelete("{id:int}")]
        [ProducesResponseType(StatusCodes.Status204NoContent)]
        [ProducesResponseType(StatusCodes.Status404NotFound)]
        public async Task<IActionResult> DeleteAsync(int id, CancellationToken ct)
        {
            var deleted = await _service.DeleteAsync(id, ct);
            return deleted ? NoContent() : NotFound();
        }
    }

    // ─── DTOs — never expose domain models directly ──────────────────────────

    public record UserDto(int Id, string Name, string Email, bool IsActive);

    public record CreateUserRequest(
        [property: System.ComponentModel.DataAnnotations.Required]
        [property: System.ComponentModel.DataAnnotations.StringLength(100)]
        string Name,
        [property: System.ComponentModel.DataAnnotations.EmailAddress]
        string Email);

    public record UpdateUserRequest(string Name, string Email, bool IsActive);

    // ─── Service abstraction — business logic lives here, not in controller ──

    public interface IUserService
    {
        Task<UserDto?> GetByIdAsync(int id, CancellationToken ct = default);
        Task<IEnumerable<UserDto>> GetAllAsync(CancellationToken ct = default);
        Task<UserDto> CreateAsync(CreateUserRequest request, CancellationToken ct = default);
        Task<bool> UpdateAsync(int id, UpdateUserRequest request, CancellationToken ct = default);
        Task<bool> DeleteAsync(int id, CancellationToken ct = default);
    }
}
